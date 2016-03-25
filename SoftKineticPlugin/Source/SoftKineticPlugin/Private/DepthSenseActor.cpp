#include "SoftKineticPluginPrivatePCH.h"
#include "DepthSenseActor.h"
#include "DepthSenseSoundWave.h"
#include "ProceduralMeshComponent.h"
#include "MediaSoundWave.h"
#include "WeakObjectPtr.h"
#include "DepthSenseTexture.h"
#include "FrameSource.h"
#include "DepthSense-public.hxx"
#include "DepthSense.hxx"
#include "CI/CI_api.h"
DEFINE_LOG_CATEGORY_STATIC(LogDepthSense, Log, All);

using namespace DepthSense;


class DepthSenseRunner : public FRunnable, public IFrameSource
{
	FWeakObjectPtr Target;
	Context context;
	DepthNode depthNode;
	ColorNode colorNode;
	AudioNode audioNode;
	FRunnableThread *Thread;
	bool bDeviceFound;

	void* m_CIHandle;;
	double m_lastTime;
	// CI data structures
	CIHeadTrackingData m_headTrackingData;
	CICameraIntrinsicParameters m_depthCameraParameters;
	CIFrameInputData m_frameData;
	CIHandTrackingData m_trackingData;
	CIFingersTrackingData m_fingersTrackingData;
	CIMovingGestureEventList m_movingGestureEvents;
	CIHandPosingGestureData m_posingGestureData;
	CIOpennessStatus m_opennessStatus[CI_HAND_COUNT];
	float m_opennessValue[CI_HAND_COUNT];
	CIMesh m_mesh[CI_HAND_COUNT];
	CICentroids m_centroids[CI_HAND_COUNT];
	CIHandGrabberData m_grabberData[CI_HAND_COUNT];


	bool bRunning;

	FCriticalSection Crit;
	int64 ColorSample;
	int64 DepthSample;
	int64 SentSample;

	// Depth Camera
	int32 DepthWidth;
	int32 DepthHeight;
	TArray<FVector> CameraSpacePoints;
	TArray<FVector2D> UVs;

	// Color camera
	int32 ColorWidth;
	int32 ColorHeight;
	TArray<uint8> Bgra;


	// Audio
	int32 Channels;
	int32 SampleRate;
	TArray<uint8> AudioData;

	TArray<int32> Triangles;
	TArray<FVector> Vertices;
	TArray<FColor> VertexColors;
	TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> SenderColorBuffer;

	TArray<FSK_Hand> Hands;
	bool HeadDetected;
	FVector HeadPosition;

public:

	/*----------------------------------------------------------------------------*/
	void onNodeConnected(Device device, Device::NodeAddedData data)
	{
		configureNode(data.node);
	}

	/*----------------------------------------------------------------------------*/
	void onNodeDisconnected(Device device, Device::NodeRemovedData data)
	{
		if (data.node.is<AudioNode>() && (data.node.as<AudioNode>() == audioNode))
			audioNode.unset();
		if (data.node.is<ColorNode>() && (data.node.as<ColorNode>() == colorNode))
			colorNode.unset();
		if (data.node.is<DepthNode>() && (data.node.as<DepthNode>() == depthNode))
			depthNode.unset();
		UE_LOG(LogDepthSense, Warning, TEXT("Node Disconnected"));
	}

	/*----------------------------------------------------------------------------*/
	void onDeviceConnected(Context context, Context::DeviceAddedData data)
	{
		if (!bDeviceFound)
		{
			data.device.nodeAddedEvent().connect(this, &DepthSenseRunner::onNodeConnected);
			data.device.nodeRemovedEvent().connect(this, &DepthSenseRunner::onNodeDisconnected);
			bDeviceFound = true;
		}
	}

	/*----------------------------------------------------------------------------*/
	void onDeviceDisconnected(Context context, Context::DeviceRemovedData data)
	{
		bDeviceFound = false;
		UE_LOG(LogDepthSense, Warning, TEXT("Device Disconnected"));
	}

	static void connectThunk(Context context, Context::DeviceAddedData data, DepthSenseRunner *This)
	{
		This->onDeviceConnected(context, data);
	}

	static void disconnectThunk(Context context, Context::DeviceRemovedData data, DepthSenseRunner *This)
	{
		This->onDeviceDisconnected(context, data);
	}

	uint32 Run() override
	{
		context = Context::create("localhost");
		context.deviceAddedEvent().connect(&DepthSenseRunner::connectThunk, this);
		context.deviceRemovedEvent().connect(&DepthSenseRunner::disconnectThunk, this);
		auto da = context.getDevices();
		if (da.size() > 0)
		{
			da[0].nodeAddedEvent().connect(this, &DepthSenseRunner::onNodeConnected);
			da[0].nodeRemovedEvent().connect(this, &DepthSenseRunner::onNodeDisconnected);
			auto na = da[0].getNodes();
			for (int32 n = 0; n < (int32)na.size(); n++) {
				configureNode(na[n]);
			}
		}
		if (bRunning)
		{
			DoRun();
		}
		delete m_frameData.colorImage;
		delete m_frameData.depthImage;
		delete m_frameData.confidenceImage;
		if (m_CIHandle != nullptr)
		{
			CI_shutdown(m_CIHandle);
			m_CIHandle = nullptr;
		}
		return 0;
	}

	uint32 DoRun()
	{		
		try
		{
			UE_LOG(LogDepthSense, Display, TEXT("Starting Nodes"));
			context.startNodes();
		}
		catch (DepthSense::Exception &e)
		{
			UE_LOG(LogDepthSense, Error, TEXT("startNodes: %s"), *FString(e.what()));
		}
		if (bRunning)
		{
			UE_LOG(LogDepthSense, Display, TEXT("Run"));
			context.run();
		}
		try 
		{
			UE_LOG(LogDepthSense, Display, TEXT("Stopping Nodes"));
			context.stopNodes();
		}
		catch (DepthSense::Exception &e)
		{
			UE_LOG(LogDepthSense, Error, TEXT("stopNodes: %s"), *FString(e.what()));
		}
		UE_LOG(LogDepthSense, Display, TEXT("Done."));
		return 0;
	}

	DepthSenseRunner(ADepthSenseActor *Actor) :
		Thread(nullptr), Target(Actor), 
		ColorWidth(0), ColorHeight(0), DepthWidth(0), DepthHeight(0),
		SampleRate(0), Channels(0), ColorSample(0), DepthSample(0)
	{
		m_lastTime = 0.0;
		m_CIHandle = nullptr;
		memset(&m_headTrackingData, 0, sizeof(CIHeadTrackingData));
		memset(&m_frameData, 0, sizeof(CIFrameInputData));
		memset(&m_trackingData, 0, sizeof(CIHandTrackingData));
		memset(&m_depthCameraParameters, 0, sizeof(CICameraIntrinsicParameters));
		memset(&m_fingersTrackingData, 0, sizeof(CIFingersTrackingData));
		memset(&m_movingGestureEvents, 0, sizeof(CIMovingGestureEventList));
		memset(&m_posingGestureData, 0, sizeof(CIHandPosingGestureData));
		memset(&m_opennessStatus, 0, sizeof(CIOpennessStatus)*CI_HAND_COUNT);
		memset(&m_opennessValue, 0, sizeof(float)*CI_HAND_COUNT);
		memset(&m_centroids, 0, sizeof(CICentroids)*CI_HAND_COUNT);
		memset(&m_mesh, 0, sizeof(CIMesh)*CI_HAND_COUNT);
		memset(&m_grabberData, 0, sizeof(CIHandGrabberData)*CI_HAND_COUNT);
	}

	void configureNode(Node node)
	{
		if ((node.is<DepthNode>()) && (!depthNode.isSet()))
		{
			depthNode = node.as<DepthNode>();
			configureDepthNode();
			context.registerNode(node);
		}

		if ((node.is<ColorNode>()) && (!colorNode.isSet()))
		{
			colorNode = node.as<ColorNode>();
			configureColorNode();
			context.registerNode(node);
		}

		if ((node.is<AudioNode>()) && (!audioNode.isSet()))
		{
			audioNode = node.as<AudioNode>();
			configureAudioNode();
			context.registerNode(node);
		}
	}

	void BeginPlay()
	{
		bRunning = true;
		Thread = FRunnableThread::Create(this, TEXT("DepthSense"));
	}

	void EndPlay()
	{
		bRunning = false;
		if (context.isSet())
		{
			try
			{
				context.quit();
			}
			catch (DepthSense::Exception &e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Exception on Context::quit %s"), *FString(e.what()));
			}
		}
		Thread->WaitForCompletion();
		delete Thread;
		
	}

	void onNewAudioSample(AudioNode node, AudioNode::NewSampleReceivedData data)
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor != nullptr)
		{
			Crit.Lock();
			Channels = data.captureConfiguration.channels;
			SampleRate = data.captureConfiguration.sampleRate;
			AudioData.Append(data.audioData, data.audioData.size());
			Crit.Unlock();
		}
	}

	void onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data)
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor != nullptr)
		{
			int32 w = 0, h = 0;
			FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);
			const int32 Count = data.colorMap.size();
			Crit.Lock();
			ColorWidth = w;
			ColorHeight = h;
			Bgra.Reset(Count);
			for (int32 i = 0; i < Count; i += 3)
			{
				Bgra.Add(data.colorMap[i]);
				Bgra.Add(data.colorMap[i + 1]);
				Bgra.Add(data.colorMap[i + 2]);
				Bgra.Add(255);
			}
			ColorSample++;
			Crit.Unlock();
		}
	}

	bool initializeCI(const DepthNode::NewSampleReceivedData &data)
	{
		DepthSense::IntrinsicParameters dsParams = data.stereoCameraParameters.depthIntrinsics;

		m_depthCameraParameters.width = dsParams.width;
		m_depthCameraParameters.height = dsParams.height;
		m_depthCameraParameters.fx = dsParams.fx;
		m_depthCameraParameters.fy = dsParams.fy;
		m_depthCameraParameters.cx = dsParams.cx;
		m_depthCameraParameters.cy = dsParams.cy;
		m_depthCameraParameters.p1 = dsParams.p1;
		m_depthCameraParameters.p2 = dsParams.p2;
		m_depthCameraParameters.k1 = dsParams.k1;
		m_depthCameraParameters.k2 = dsParams.k2;
		m_depthCameraParameters.k3 = dsParams.k3;

		FString Dir = FPaths::GameDir();
		Dir += "/Plugins/SoftKineticPlugin/Thirdparty/CIlib/data";

		CIReturnCode ret = CI_initialize(TCHAR_TO_ANSI(*Dir), &m_depthCameraParameters, NULL, NULL, &m_CIHandle);

		if (ret != CI_SUCCESS)
		{
			//std::cerr << "Could not initialize CILib. Reason:" << CI_getErrorDescription(m_CIHandle, ret) << std::endl;
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't initialize CI: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, ret)));
			return false;
		}

		ret = CI_setExperienceMode(m_CIHandle, CI_ORIGINAL_CI_DS_325);

		if (ret != CI_SUCCESS)
		{
			//std::cerr << "Could not set the experience mode to CI DS325. Reason:" << CI_getErrorDescription(m_CIHandle, ret) << std::endl;
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't set experience mode to CI DS325: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, ret)));
			return false;
		}


		return true;

	}


	void onNewDepthSampleCI(DepthNode Node, DepthNode::NewSampleReceivedData data)
	{
		unsigned int imgWidth = data.stereoCameraParameters.depthIntrinsics.width;
		unsigned int imgHeight = data.stereoCameraParameters.depthIntrinsics.height;

		if (m_frameData.depthImage == NULL)
		{
			m_frameData.depthImage = new int16_t[imgWidth * imgHeight];

			//first frame, init the CI pipeline
			bool keepGoing = initializeCI(data);

			if (!keepGoing)
			{
				//std::cerr << "Disconnecting from DepthSense driver." << std::endl;
				//stopCamera();
				return;
			}

			m_frameData.timeSinceLastFrame = 0.f;
			m_lastTime = FPlatformTime::Seconds() * 1000.0f;
		}
		else
		{
			double currentTime = FPlatformTime::Seconds() * 1000.0f;
			m_frameData.timeSinceLastFrame = currentTime - m_lastTime;
			m_lastTime = currentTime;
		}

		memcpy(m_frameData.depthImage, data.depthMap, imgWidth*imgHeight*sizeof(int16_t));

		const int16_t* confidence = NULL;
		unsigned int confidenceBufferSize = 0;

		confidence = (const int16_t*)(data.confidenceMap);
		confidenceBufferSize = (unsigned int)data.confidenceMap.size();

		if (confidenceBufferSize != 0)
		{
			if (m_frameData.confidenceImage == NULL)
			{
				m_frameData.confidenceImage = new int16_t[confidenceBufferSize];
			}

			memcpy(m_frameData.confidenceImage, confidence, confidenceBufferSize * sizeof(int16_t));

		}
		else
		{
			//std::cerr << "DepthSense camera: invalid confidence buffer size" << std::endl;
		}

		CIReturnCode status = CI_setInputData(m_CIHandle, &m_frameData);

		if (status != CI_SUCCESS)
		{
			//std::cerr << "Could not set input data. Reason: " << CI_getErrorDescription(m_CIHandle, status) << std::endl;
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't set input data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
			return;
		}

		CIFilteredData outputFilteredData;
		status = CI_updateFilteredData(m_CIHandle, &outputFilteredData);
		if (status != CI_SUCCESS)
		{
			//std::cerr << "Could not update filter data. Reason: " << CI_getErrorDescription(m_CIHandle, status) << std::endl;
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't update filter data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
			return;
		}

		status = CI_updateHandTrackingData(m_CIHandle, &m_trackingData);

		if (status != CI_SUCCESS)
		{
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't update hand tracking data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
			return;
		}

		status = CI_computeFingersTrackingData(m_CIHandle, &m_fingersTrackingData);
		if (status != CI_SUCCESS)
		{
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't update head finger tracking data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
		}
		status = CI_computeHandOpenness(m_CIHandle, &m_opennessStatus[0], &m_opennessValue[0]);
		if (status != CI_SUCCESS)
		{
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't update hand openness data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
		}
		if (CI_updateHeadTrackingData(m_CIHandle, &m_headTrackingData) != CI_SUCCESS)
		{
			UE_LOG(LogDepthSense, Error, TEXT("Couldn't update head tracking data: %s"), ANSI_TO_TCHAR(CI_getErrorDescription(m_CIHandle, status)));
		}
		/*
		CI_updateMovingGestureEventList(m_CIHandle, &m_movingGestureEvents);
		CI_updateHandPosingGestureData(m_CIHandle, &m_posingGestureData);
		for (unsigned int i = 0; i<CI_HAND_COUNT; ++i)
		{
			CI_computeMesh(m_CIHandle, i, &m_mesh[i]);
			CI_computeCentroids(m_CIHandle, i, &m_centroids[i]);
			CI_updateGrabberData(m_CIHandle, i, &m_grabberData[i]);
		}
		*/
	}


	void onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data)
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor != nullptr) {
			int32 w, h;
			FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);
			int32 Count = data.vertices.size();
			Crit.Lock();
			onNewDepthSampleCI(node, data);
			DepthWidth = w;
			DepthHeight = h;
			CameraSpacePoints.Reset(Count);
			UVs.Reset(Count);
			const float Scale = 0.1f; // mm to cm
			for (int32 i = 0; i < Count; i++)
			{
				if (data.vertices[i].z > 32000)
				{
					CameraSpacePoints.Add(FVector(0, 0, 0));
				}
				else
				{
					float Right = data.vertices[i].x * Scale;
					float Up = data.vertices[i].y * Scale;
					float Forward = data.vertices[i].z * Scale;
					CameraSpacePoints.Add(FVector(Forward, Right, Up));
				}
				UVs.Add(FVector2D(data.uvMap[i].u, data.uvMap[i].v));
			}
			DepthSample++;
			Crit.Unlock();
		}
	}

	static FVector ToVector(const CIVector3 &V)
	{
		const float Scale = 100.0f; // meters to cm
		float Right = V.x * Scale;
		float Up = V.y * Scale;
		float Forward = V.z * Scale;
		return FVector(Forward, Right, Up);
	}

	static ESK_HandSide ToHandSide(const CIHandSide &HandSide)
	{
		ESK_HandSide Result = ESK_HandSide::SK_Unknown;
		switch (HandSide)
		{
		case CIHandSide::CI_HANDSIDE_LEFT:
			Result = ESK_HandSide::SK_Left;
			break;
		case CIHandSide::CI_HANDSIDE_RIGHT:
			Result = ESK_HandSide::SK_Right;
			break;
		case CIHandSide::CI_HANDSIDE_UNKNOWN:
			Result = ESK_HandSide::SK_Unknown;
			break;

		}
		return Result;
	}

	static ESK_TrackingStatus ToTrackingStatus(int32 Status)
	{
		ESK_TrackingStatus Result = ESK_TrackingStatus::SK_Inactive;
		switch (Status)
		{
		case CITrackingStatus::CI_STATUS_DETECTED:
			Result = ESK_TrackingStatus::SK_Detected;
			break;
		case CITrackingStatus::CI_STATUS_EXTRAPOLATED:
			Result = ESK_TrackingStatus::SK_Extrapolated;
			break;
		case CITrackingStatus::CI_STATUS_INACTIVE:
			Result = ESK_TrackingStatus::SK_Inactive;
			break;
		case CITrackingStatus::CI_STATUS_TRACKED:
			Result = ESK_TrackingStatus::SK_Tracked;
			break;

		}
		return Result;
	}

	virtual int64 GetFrameData(
		int64 LastFrame,
		TArray<int32> &Triangles,
		TArray<FVector> &Vertices,
		TArray<FColor> &VertexColors,
		TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> &Camera,
		int32 &ColorWidth,
		int32 &ColorHeight,
		TArray<uint8> &AudioData,
		int32 &Channels,
		int32 &SampleRate,
		TArray<FSK_Hand> &Hands,
		bool &HeadDetected,
		FVector &HeadPosition) override
	{
		int64 Result;
		Crit.Lock();
		if (LastFrame < DepthSample)
		{

			ColorWidth = this->ColorWidth;
			ColorHeight = this->ColorHeight;
			GenerateVertexData(DepthWidth, DepthHeight, ColorWidth, ColorHeight, Bgra, CameraSpacePoints, UVs, Triangles, Vertices, VertexColors);
			if (!Camera.IsValid())
			{
				Camera = TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe>(new TArray<uint8>());
			}
			*Camera = Bgra;
			if (this->AudioData.Num() > 0 && !(&AudioData == &this->AudioData))
			{
				AudioData.Append(this->AudioData);
				this->AudioData.Reset();
			}
			Channels = this->Channels;
			SampleRate = this->SampleRate;
			Hands.SetNum(CI_HAND_COUNT);
			for (int i = 0; i < CI_HAND_COUNT; i++)
			{
				auto &Hand0 = this->m_fingersTrackingData.hands[i];
				auto &Hand1 = this->m_trackingData.hands[i];
				FSK_Hand &Target = Hands[i];
				Target.Fingers.SetNum(CI_FINGER_COUNT);
				Target.ForearmPosition = ToVector(Hand1.forearmPosition3D);
				Target.MassCenter = ToVector(Hand1.massCenter3D);
				Target.PalmNormal = ToVector(Hand1.palmNormal);
				Target.TipPosition = ToVector(Hand1.tipPosition3D);
				Target.HandSide = ToHandSide(Hand1.handSide);
				Target.TrackingStatus = ToTrackingStatus(Hand1.status);
				for (int j = 0; j < CI_FINGER_COUNT; j++)
				{
					const auto &FingerPos = Hand0.fingerTipPosition3D[j];
					Target.Fingers[j].FingertipPosition = ToVector(FingerPos);
					Target.Fingers[j].FingertipRadius = Hand0.fingerTipRadius3D[j];
					Target.Fingers[j].TrackingStatus = ToTrackingStatus(Hand0.fingerStatus[j]);
				}
			}
			HeadDetected = m_headTrackingData.headDetected == CIBool::CI_TRUE;
			HeadPosition = ToVector(m_headTrackingData.headPosition3D);
		}
		Result = DepthSample;
		try
		{
			configureFilters();
		}
		catch (DepthSense::Exception& e)
		{
			UE_LOG(LogDepthSense, Error, TEXT("configureFilters: %s"), *FString(e.what()));
		}
		Crit.Unlock();
		return Result;
	}

	void GenerateVertexData(
		int32 DepthWidth,
		int32 DepthHeight,
		int32 ColorWidth,
		int32 ColorHeight,
		const TArray<uint8> &ColorBuffer,
		const TArray<FVector> &CameraSpacePoints,
		const TArray<FVector2D> &UVs,
		TArray<int32> &Triangles,
		TArray<FVector> &Vertices,
		TArray<FColor> &VertexColors)
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		Triangles.Reset();
		Vertices.Reset();
		VertexColors.Reset();
		if (Actor == nullptr || ColorWidth == 0 || ColorHeight == 0 || DepthWidth == 0 || DepthHeight == 0)
		{
			return;
		}
		const float Resolution = Actor->Resolution;
		const float ViewportWidth = Actor->ViewportWidth;
		const float ViewportHeight = Actor->ViewportHeight;
		const float MaxDistanceInCm = Actor->MaxDistanceInMeters * 100.0f;
		const float MinDistanceInCm = Actor->MinDistanceInMeters * 100.0f;
		const int32 MaxEdgeLength = Actor->MaxEdgeLength;
		const int step = 1;
		const int h = FMath::RoundToInt(DepthHeight * FMath::Clamp(Resolution, 0.1f, 1.0f));
		const int w = FMath::RoundToInt(DepthWidth * FMath::Clamp(Resolution, 0.1f, 1.0f));
		const int startx = FMath::RoundToInt((w - FMath::Clamp(ViewportWidth, 0.0f, 1.0f)*w) / 2.0f);
		const int starty = FMath::RoundToInt((h - FMath::Clamp(ViewportHeight, 0.0f, 1.0f)*h) / 2.0f);
		int32 Dropped = 0;
		FVector P[2][2];
		FColor C[2][2];
		const float iw = 1.0 / w;
		const float ih = 1.0 / h;


		for (int y = starty; y < h - step - starty; y += step)
		{
			for (int x = startx; x < w - step - startx; x += step)
			{
				const float scale = 1.0f;
				bool skip = false;
				for (int32 ix = 0; ix < 2 && !skip; ix++)
				{
					for (int32 iy = 0; iy < 2; iy++)
					{
						const int32 X0 = (x + ix*step);
						const int32 Y0 = (y + iy*step);
						const int32 X = FMath::RoundToInt((X0 * iw)*DepthWidth);
						const int32 Y = FMath::RoundToInt((Y0 * ih)*DepthHeight);
						const int32 DepthIndex = Y  * DepthWidth + X;

						const FVector &cameraSpacePoint = CameraSpacePoints[DepthIndex];
						if (cameraSpacePoint.X == 0)
						{
							skip = true;
							break;
						}
						const FVector2D &uv = UVs[DepthIndex];
						const float dist = FMath::Sqrt(cameraSpacePoint.X * cameraSpacePoint.X +
							cameraSpacePoint.Y * cameraSpacePoint.Y +
							cameraSpacePoint.Z * cameraSpacePoint.Z);
						if (dist > MinDistanceInCm && dist <= MaxDistanceInCm)
						{
							const uint32 Row = FMath::RoundToInt(uv.Y * ColorHeight);
							const uint32 Col = FMath::RoundToInt(uv.X * ColorWidth);
							const uint8 *bgra = &(ColorBuffer.GetData()[(Row * ColorWidth + Col) * 4]);
							FColor &Color = C[ix][iy];
							Color.B = bgra[0];
							Color.G = bgra[1];
							Color.R = bgra[2];
							Color.A = 0xff;
							P[ix][iy] = cameraSpacePoint;
						}
						else
						{
							skip = true;
							break;
						}

					}
				}
				if (skip)
				{
					Dropped++;
					continue;
				}
				const FVector &P00 = P[0][0];
				const FVector &P01 = P[0][1];
				const FVector &P10 = P[1][0];
				const FVector &P11 = P[1][1];
				const int32 max_edge_len = MaxEdgeLength;
				if (((P00.X > 0) && (P01.X > 0) && (P10.X > 0) && (P11.X > 0) && (P01.X > 0) && (P10.X > 0) &&// check for non valid values
					(abs(P00.X - P01.X) < max_edge_len) &&
					(abs(P10.X - P01.X) < max_edge_len) &&
					(abs(P11.X - P01.X) < max_edge_len) &&
					(abs(P10.X - P01.X) < max_edge_len)))
				{
					const int32 Next = Vertices.Num();

					Vertices.Add(P00);
					Vertices.Add(P01);
					Vertices.Add(P10);

					Triangles.Add(Next);
					Triangles.Add(Next + 1);
					Triangles.Add(Next + 2);


					VertexColors.Add(C[0][0]);
					VertexColors.Add(C[0][1]);
					VertexColors.Add(C[1][0]);

					//Vertices.Add(P01);
					Triangles.Add(Next + 1);
					Vertices.Add(P11);
					Triangles.Add(Next + 3);
					//Vertices.Add(P10);
					Triangles.Add(Next + 2);
					//VertexColors.Add(C[0][1]);
					VertexColors.Add(C[1][1]);
					//VertexColors.Add(C[1][0]);
				}
			}
		}
	}

	void configureColorNode()
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor != nullptr)
		{

			colorNode.newSampleReceivedEvent().connect(this, &DepthSenseRunner::onNewColorSample);
			ColorNode::Configuration config = colorNode.getConfiguration();
			config.frameFormat = FRAME_FORMAT_VGA;
			config.framerate = Actor->CameraFPS;
			config.compression = COMPRESSION_TYPE_MJPEG;
			config.powerLineFrequency = POWER_LINE_FREQUENCY_50HZ;
			try {
				colorNode.setEnableColorMap(true);
				context.requestControl(colorNode, 0);
				colorNode.setConfiguration(config);
			}
			catch (ArgumentException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Argument Exception %s"), *FString(e.what()));
			}
			catch (UnauthorizedAccessException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Unauthorized Access Exception: %s\n"), *FString(e.what()));
			}
			catch (IOException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("IO Exception : %s\n"), *FString(e.what()));
			}
			catch (InvalidOperationException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Invalid Operation Exception : %s\n"), *FString(e.what()));
			}
			catch (ConfigurationException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Configuration Exception : %s\n"), *FString(e.what()));
			}
			catch (StreamingException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Streaming Exception : %s\n"), *FString(e.what()));
			}
			catch (TimeoutException&)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Timeout Exception : %s\n"));
			}
			context.releaseControl(colorNode);
		}

	}

	void configureAudioNode()
	{
		audioNode.newSampleReceivedEvent().connect(this, &DepthSenseRunner::onNewAudioSample);

		AudioNode::Configuration config = audioNode.getConfiguration();
		config.sampleRate = 44100;

		try
		{
			context.requestControl(audioNode, 0);

			audioNode.setConfiguration(config);

			audioNode.setInputMixerLevel(0.5f);
		}
		catch (DepthSense::Exception& e)
		{
			UE_LOG(LogDepthSense, Error, TEXT("Exception %s"), *FString(e.what()));
		}
		context.releaseControl(audioNode);

	}

	FSK_Filter ConfiguredFilters;

	void configureFilters()
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor == nullptr || !depthNode.isSet())
		{
			return;
		}
		const FSK_Filter Filters = Actor->Filters;
		if (memcmp(&ConfiguredFilters, &Filters, sizeof(FSK_Filter)) == 0)
		{
			return;
		}
		ConfiguredFilters = Filters;
		context.requestControl(depthNode, 1000);
		depthNode.setEnableFilter1(Filters.Filter1.bEnable);
		depthNode.setEnableFilter2(Filters.Filter2.bEnable);
		depthNode.setEnableFilter3(Filters.Filter3.bEnable);
		depthNode.setEnableFilter4(Filters.Filter4.bEnable);
		depthNode.setEnableFilter5(Filters.Filter5.bEnable);
		depthNode.setEnableFilter6(Filters.Filter6.bEnable);
		depthNode.setEnableFilter7(Filters.Filter7.bEnable);
		depthNode.setEnableFilter8(Filters.Filter8.bEnable);
		depthNode.setEnableFilter1(Filters.Filter1.bEnable);
		depthNode.setEnableFilter2(Filters.Filter2.bEnable);
		depthNode.setEnableFilter3(Filters.Filter3.bEnable);
		depthNode.setEnableFilter4(Filters.Filter4.bEnable);
		//depthNode.setEnableFilter5(Filters.Filter5.bEnable);
		//depthNode.setEnableFilter6(Filters.Filter6.bEnable);
		//depthNode.setEnableFilter7(Filters.Filter7.bEnable);
		depthNode.setEnableFilter8(Filters.Filter8.bEnable);
		depthNode.setEnableFilter9(Filters.Filter9.bEnable);
		//depthNode.setEnableFilter10(Filters.Filter10.bEnable);

		depthNode.setFilter1Parameter1(Filters.Filter1.Parameter1);
		depthNode.setFilter1Parameter2(Filters.Filter1.Parameter2);
		depthNode.setFilter1Parameter3(Filters.Filter1.Parameter3);
		depthNode.setFilter1Parameter4(Filters.Filter1.Parameter4);

		depthNode.setFilter2Parameter1(Filters.Filter2.Parameter1);

		depthNode.setFilter3Parameter1(Filters.Filter3.Parameter1);
		depthNode.setFilter3Parameter2(Filters.Filter3.Parameter2);
		depthNode.setFilter3Parameter3(Filters.Filter3.Parameter3);

		depthNode.setFilter4Parameter1(Filters.Filter4.Parameter1);

		//depthNode.setFilter5Parameter1(Filters.Filter5.Parameter1);

		//depthNode.setFilter6Parameter1(Filters.Filter6.Parameter1);
		//depthNode.setFilter6Parameter2(Filters.Filter6.Parameter2);
		//depthNode.setFilter6Parameter3(Filters.Filter6.Parameter3);
		//depthNode.setFilter6Parameter4(Filters.Filter6.Parameter4);

		//depthNode.setFilter7Parameter1(Filters.Filter7.Parameter1);
		//depthNode.setFilter7Parameter2(Filters.Filter7.Parameter2);

		depthNode.setFilter8Parameter1(Filters.Filter8.Parameter1);

		depthNode.setFilter9Parameter1(Filters.Filter9.Parameter1);
		depthNode.setFilter9Parameter2(Filters.Filter9.Parameter2);
		depthNode.setFilter9Parameter3(Filters.Filter9.Parameter3);
		depthNode.setFilter9Parameter4(Filters.Filter9.Parameter4);

		//depthNode.setFilter10Parameter1(Filters.Filter10.Parameter1);
		//depthNode.setFilter10Parameter2(Filters.Filter10.Parameter2);
		//depthNode.setFilter10Parameter3(Filters.Filter10.Parameter3);
		//depthNode.setFilter10Parameter4(Filters.Filter10.Parameter4);depthNode.setEnableFilter9(Filters.Filter9.bEnable);
		context.releaseControl(depthNode);

	}

	void configureDepthNode()
	{
		ADepthSenseActor *Actor = (ADepthSenseActor*)Target.Get();
		if (Actor != nullptr)
		{

			depthNode.newSampleReceivedEvent().connect(this, &DepthSenseRunner::onNewDepthSample);
			DepthNode::Configuration config = depthNode.getConfiguration();
			config.frameFormat = FRAME_FORMAT_QVGA;
			config.framerate = Actor->CameraFPS;
			config.mode = Actor->CameraMode == ESK_CameraMode::SK_LongRange ? DepthNode::CAMERA_MODE_LONG_RANGE : DepthNode::CAMERA_MODE_CLOSE_MODE;
			config.saturation = true;
			depthNode.setEnableDepthMap(true);
			depthNode.setEnableVertices(true);
			depthNode.setEnableUvMap(true);
			depthNode.setEnableConfidenceMap(true);
			//depthNode.setEnableDenoising(false);
			try
			{
				context.requestControl(depthNode, 0);
				depthNode.setConfiguration(config);
			}
			catch (ArgumentException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Argument Exception %s"), *FString(e.what()));
			}
			catch (UnauthorizedAccessException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Unauthorized Access Exception: %s\n"), *FString(e.what()));
			}
			catch (IOException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("IO Exception : %s\n"), *FString(e.what()));
			}
			catch (InvalidOperationException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Invalid Operation Exception : %s\n"), *FString(e.what()));
			}
			catch (ConfigurationException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Configuration Exception : %s\n"), *FString(e.what()));
			}
			catch (StreamingException& e)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Streaming Exception : %s\n"), *FString(e.what()));
			}
			catch (TimeoutException&)
			{
				UE_LOG(LogDepthSense, Error, TEXT("Timeout Exception : %s\n"));
			}
			context.releaseControl(depthNode);

		}
	}
};



ADepthSenseActor::ADepthSenseActor(const FObjectInitializer &Init) :
	Runner(nullptr)
	, Camera(nullptr)
	, AudioOutput(nullptr)
	, CameraFPS(25)
	, CameraMode(ESK_CameraMode::SK_ShortRange)
	, ViewportWidth(1.0f)
	, ViewportHeight(1.0f)
	, MinDistanceInMeters(0)
	, MaxDistanceInMeters(10)
	, Resolution(1.0f)
	, MaxEdgeLength(4)
	, FrameIndex(0)
	, bMeshPending(false)
{
	MeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	PrimaryActorTick.bCanEverTick = true;
}

void ADepthSenseActor::BeginPlay()
{
	Runner = new DepthSenseRunner(this);
	CameraOutput = NewObject<UDepthSenseTexture>();
	CameraOutput->SetDimensions(FIntPoint(640, 480));
	CameraOutput->UpdateResource();
	FrameSource = Runner;
	Runner->BeginPlay();
	Super::BeginPlay();
}


void ADepthSenseActor::Tick(float  DeltaSeconds)
{
	// process pending commands
	CommandDelegate Command;
	while (Commands.Dequeue(Command))
	{
		Command.Execute();
	}

	Runner->Tick();
	if (FrameSource != nullptr && !bMeshPending)
	{
		int32 ColorWidth, ColorHeight;
		int64 NextIndex = FrameSource->GetFrameData(FrameIndex,
			Triangles, Vertices, VertexColors,
			CurrentCameraFrame, ColorWidth, ColorHeight,
			AudioData, Channels, SampleRate,
			Hands, HeadDetected, HeadPosition);

		if (FrameIndex < NextIndex)
		{

			FrameIndex = NextIndex;
			// mesh reconstruction
			if (Triangles.Num() > 0)
			{
				MeshComp->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, EnablePhysics);
			}
			else
			{
				MeshComp->ClearAllMeshSections();
			}
			// camera feed
			if (CameraOutput != nullptr && ColorWidth != 0 && ColorHeight != 0)
			{
				FIntPoint Dim = CameraOutput->GetDimensions();
				if (Dim.X != ColorWidth || Dim.Y != ColorHeight)
				{
					CameraOutput->SetDimensions(FIntPoint(ColorWidth, ColorHeight));
					CameraOutput->UpdateResource();
				}
				CameraOutput->SetCurrentFrame(CurrentCameraFrame);
			}
			// audio feed
			if (AudioOutput != nullptr && AudioData.Num() > 0)
			{
				AudioOutput->NumChannels = Channels;
				AudioOutput->SampleRate = SampleRate;
				AudioOutput->EnqueuePCMData(AudioData.GetData(), AudioData.Num());			
			}
			AudioData.Reset();
			bMeshPending = true;
		}
	}
	else
	{
		bMeshPending = false;
	}
	Super::Tick(DeltaSeconds);
}

void ADepthSenseActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Runner->EndPlay();
	delete Runner;
	Runner = nullptr;
	Super::EndPlay(EndPlayReason);
}
