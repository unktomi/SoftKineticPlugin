#include "SoftkineticPluginPrivatePCH.h"
#include "DepthSenseTexture.h"
#include "Engine/EngineTypes.h"
#include "DeviceProfiles/DeviceProfile.h"


/**
 * FTextureResource type for movie textures.
 */
class FDepthSenseTextureResource
	: public FTextureResource
	, public FRenderTarget
	, public FDeferredUpdateResource
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InOwner The DepthSense texture object to create a resource for (must not be nullptr).
	 */
	FDepthSenseTextureResource(const class UDepthSenseTexture* InOwner);

public:

	// FTextureResource overrides

	virtual void InitDynamicRHI() override;
	virtual void ReleaseDynamicRHI() override;

public:

	// FRenderTarget overrides

	virtual FIntPoint GetSizeXY() const override;

public:

	// FDeferredUpdateResource overrides

	virtual void UpdateDeferredResource(FRHICommandListImmediate& RHICmdList, bool bClearRenderTarget = true) override;

private:

	/** Whether the resource was cleared last frame. */
	bool Cleared;

	/** The color that the resource was cleared with. */
	FLinearColor LastClearColor;

	/** The playback time of the last drawn video frame. */
	FTimespan LastFrameTime;

	/** The UTextureRenderTarget2D which this resource represents. */
	const UDepthSenseTexture *Owner;

	/** Texture resource used for rendering with and resolving to. */
	FTexture2DRHIRef Texture2DRHI;

};

/* FDepthSenseTextureResource structors
*****************************************************************************/

FDepthSenseTextureResource::FDepthSenseTextureResource(const class UDepthSenseTexture* InOwner)
	: Cleared(false)
	, LastFrameTime(FTimespan::MinValue())
	, Owner(InOwner)
{
}


/* FTextureResource overrides
*****************************************************************************/

void FDepthSenseTextureResource::InitDynamicRHI()
{

	if ((Owner->GetSurfaceWidth() > 0) && (Owner->GetSurfaceHeight() > 0))
	{
		// Create the RHI texture. Only one mip is used and the texture is targetable or resolve.
		uint32 TexCreateFlags = Owner->SRGB ? TexCreate_SRGB : 0;
		TexCreateFlags |= TexCreate_Dynamic | TexCreate_NoTiling;
		FRHIResourceCreateInfo CreateInfo;

		RHICreateTargetableShaderResource2D(
			Owner->GetSurfaceWidth(),
			Owner->GetSurfaceHeight(),
			Owner->GetFormat(),
			1,
			TexCreateFlags,
			TexCreate_RenderTargetable,
			false,
			CreateInfo,
			RenderTargetTextureRHI,
			Texture2DRHI
			);

		TextureRHI = (FTextureRHIRef&)Texture2DRHI;
		RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, TextureRHI);

		// add to the list of global deferred updates (updated during scene rendering)
		// since the latest decoded movie frame is rendered to this media texture target
		AddToDeferredUpdateList(false);
	}

	// Create the sampler state RHI resource.
	FSamplerStateInitializerRHI SamplerStateInitializer(
		(ESamplerFilter)UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter(Owner),
		Owner->AddressX == TA_Wrap ? AM_Wrap : (Owner->AddressX == TA_Clamp ? AM_Clamp : AM_Mirror),
		Owner->AddressY == TA_Wrap ? AM_Wrap : (Owner->AddressY == TA_Clamp ? AM_Clamp : AM_Mirror),
		AM_Wrap
		);

	SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);
}


void FDepthSenseTextureResource::ReleaseDynamicRHI()
{
	// release the FTexture RHI resources here as well
	ReleaseRHI();

	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, FTextureRHIParamRef());
	Texture2DRHI.SafeRelease();
	RenderTargetTextureRHI.SafeRelease();

	// remove from global list of deferred updates
	RemoveFromDeferredUpdateList();
}


/* FRenderTarget overrides
*****************************************************************************/

FIntPoint FDepthSenseTextureResource::GetSizeXY() const
{
	return Owner->GetDimensions();
}


/* FDeferredUpdateResource overrides
*****************************************************************************/

void FDepthSenseTextureResource::UpdateDeferredResource(FRHICommandListImmediate& RHICmdList, bool bClearRenderTarget/*=true*/)
{
	TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> CurrentFrame = Owner->GetCurrentFrame();
	if (CurrentFrame.IsValid())
	{
		uint32 Stride = 0;
		FRHITexture2D* Texture2D = TextureRHI->GetTexture2D();
		uint8* TextureBuffer = (uint8*)RHILockTexture2D(Texture2D, 0, RLM_WriteOnly, Stride, false);

		FMemory::Memcpy(TextureBuffer, CurrentFrame->GetData(), CurrentFrame->Num());
		RHIUnlockTexture2D(Texture2D, 0, false);

		//      LastFrameTime = CurrentFrameTime;
		Cleared = false;
	}
	else if (!Cleared || (LastClearColor != Owner->ClearColor))
	{
		// clear texture if video track selected
		FRHICommandListImmediate& CommandList = FRHICommandListExecutor::GetImmediateCommandList();

		SetRenderTarget(CommandList, RenderTargetTextureRHI, FTextureRHIRef());
		CommandList.SetViewport(0, 0, 0.0f, Owner->GetSurfaceWidth(), Owner->GetSurfaceHeight(), 1.0f);
		CommandList.Clear(true, Owner->ClearColor, false, 0.f, false, 0, FIntRect());
		CommandList.CopyToResolveTarget(Texture2DRHI, TextureRHI, true, FResolveParams());

		LastClearColor = Owner->ClearColor;
		Cleared = true;
	}
}


UDepthSenseTexture::UDepthSenseTexture(const FObjectInitializer &Init) :
Super(Init)
, ClearColor(FLinearColor::Black)
{
	SampleInfo = new FSampleInfo();
	SampleInfo->Dimensions = FIntPoint(0, 0);
}

UDepthSenseTexture::~UDepthSenseTexture()
{
	delete SampleInfo;
}

/* UTexture  overrides
*****************************************************************************/

FTextureResource* UDepthSenseTexture::CreateResource()
{
	return new FDepthSenseTextureResource(this);
}


EMaterialValueType UDepthSenseTexture::GetMaterialType()
{
	return MCT_Texture2D;
}


float UDepthSenseTexture::GetSurfaceWidth() const
{
	return SampleInfo->Dimensions.X;
}

float UDepthSenseTexture::GetSurfaceHeight() const
{
	return SampleInfo->Dimensions.Y;
}



/* UObject  overrides
*****************************************************************************/

void UDepthSenseTexture::BeginDestroy()
{
	Super::BeginDestroy();

	// synchronize with the rendering thread by inserting a fence
	if (!ReleasePlayerFence)
	{
		ReleasePlayerFence = new FRenderCommandFence();
	}

	ReleasePlayerFence->BeginFence();
}


void UDepthSenseTexture::FinishDestroy()
{
	delete ReleasePlayerFence;
	ReleasePlayerFence = nullptr;

	Super::FinishDestroy();
}



SIZE_T UDepthSenseTexture::GetResourceSize(EResourceSizeMode::Type Mode)
{
	auto &Dim = SampleInfo->Dimensions;
	return Dim.X * Dim.Y * 4;
}


bool UDepthSenseTexture::IsReadyForFinishDestroy()
{
	// ready to call FinishDestroy if the flushing fence has been hit
	return (Super::IsReadyForFinishDestroy() && ReleasePlayerFence && ReleasePlayerFence->IsFenceComplete());
}
