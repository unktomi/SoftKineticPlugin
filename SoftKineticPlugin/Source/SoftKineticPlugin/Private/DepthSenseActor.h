#pragma once
#include "Engine.h"
#include "Array.h"
#include "Vector.h"
#include "ProceduralMeshComponent.h"
#include "FrameSource.h"
#include "DepthSenseActor.generated.h"

UENUM(BlueprintType)
enum class ESK_TrackingStatus : uint8
{
	SK_Inactive UMETA(DisplayName = "Inactive"),
	SK_Detected UMETA(DisplayName = "Detected"),
	SK_Tracked UMETA(DisplayName = "Tracked"),
	SK_Extrapolated UMETA(DisplayName = "Extrapolated")
};



USTRUCT(BlueprintType)
struct FSK_Finger
{
	GENERATED_USTRUCT_BODY();
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		FVector FingertipPosition;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		float FingertipRadius;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		ESK_TrackingStatus TrackingStatus;

};


UENUM(BlueprintType)
enum class ESK_HandSide : uint8
{
	SK_Left UMETA(DisplayName = "Left"),
	SK_Right UMETA(DisplayName = "Right"),
	SK_Unknown UMETA(DisplayName = "Unknown")
};

USTRUCT(BlueprintType)
struct FSK_Hand
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		TArray<FSK_Finger> Fingers;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		FVector MassCenter;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		FVector TipPosition;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		FVector ForearmPosition;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		/// The 3D palm normal, in depth camera space
		FVector PalmNormal;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		/// Whether the hand was detected as a right or left hand.
		ESK_HandSide HandSide;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		ESK_TrackingStatus TrackingStatus;
};


UENUM(BlueprintType)
enum class ESK_CameraMode : uint8
{
	SK_ShortRange UMETA(DisplayName="Short Range"),
	SK_LongRange UMETA(DisplayName="Long Range")
};

USTRUCT()
struct FSK_Filter1
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter1(): bEnable(false), Parameter1(16384), Parameter2(4000), Parameter3(200), Parameter4(2000)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter4;
};

USTRUCT()
struct FSK_Filter2
{
	GENERATED_USTRUCT_BODY()
		FSK_Filter2() : Parameter1(6)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
};

USTRUCT()
struct FSK_Filter3
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter3() : bEnable(false), Parameter1(0.3f), Parameter2(0.8f), Parameter3(4000)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter3;
};

USTRUCT()
struct FSK_Filter4
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter4() : bEnable(false), Parameter1(1638)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
};


USTRUCT()
struct FSK_Filter5
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter5() : bEnable(false), Parameter1(3000)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
};

USTRUCT()
struct FSK_Filter6
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter6() : bEnable(false), Parameter1(1), Parameter2(1), Parameter3(1), Parameter4(1)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter4;
};

USTRUCT()
struct FSK_Filter7
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter7() : bEnable(false), Parameter1(0.0438f), Parameter2(500.0f), Parameter3(4000.0f), Parameter4(0.1212f)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter4;
};

USTRUCT()
struct FSK_Filter8
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter8() : bEnable(false), Parameter1(109)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
};

USTRUCT()
struct FSK_Filter9
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter9() : bEnable(false), Parameter1(10), Parameter2(100), Parameter3(10), Parameter4(.1f)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Parameter4;
};

USTRUCT()
struct FSK_Filter10
{
	GENERATED_USTRUCT_BODY()
	FSK_Filter10() : bEnable(false), Parameter1(1), Parameter2(0), Parameter3(0), Parameter4(4)
	{
	}
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		bool bEnable;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 Parameter4;
};



USTRUCT()
struct FSK_Filter
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter1 Filter1;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter2 Filter2;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter3 Filter3;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter4 Filter4;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter5 Filter5;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter6 Filter6;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter7 Filter7;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter8 Filter8;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter9 Filter9;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		FSK_Filter10 Filter10;
};

UCLASS(BlueprintType, Blueprintable)
class ADepthSenseActor : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
	class UDepthSenseTexture *CameraOutput;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
	class UDepthSenseSoundWave *AudioOutput;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
	class UProceduralMeshComponent *MeshComp;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
	class UDepthSenseTexture *Camera;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
		int32 CameraFPS;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
		FSK_Filter Filters;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
		ESK_CameraMode CameraMode;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float ViewportWidth;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float ViewportHeight;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float MinDistanceInMeters;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float MaxDistanceInMeters;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		float Resolution;
	UPROPERTY(Category = "DepthSense", EditAnywhere)
		int32 MaxEdgeLength;
	UPROPERTY(Category = "DepthSense", EditAnywhere, BlueprintReadOnly)
		bool EnablePhysics;

	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		TArray<FSK_Hand> Hands;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		bool HeadDetected;
	UPROPERTY(Category = "CIlib", EditAnywhere, BlueprintReadOnly)
		FVector HeadPosition;

	/** Events have to occur on the main thread, so we have this queue to feed the ticker */
	DECLARE_DELEGATE(CommandDelegate)
	TQueue<CommandDelegate, EQueueMode::Mpsc> Commands;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	int32 Channels;
	int32 SampleRate;
	TArray<uint8> AudioData;
	class DepthSenseRunner *Runner;
	IFrameSource *FrameSource;
	TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> CurrentCameraFrame;
	int64 FrameIndex;
	bool bMeshPending;
};
