#pragma once
#include "Array.h"

struct FSK_Hand;

class IFrameSource
{
public:

	virtual void Tick() {};
	virtual void Close() {};
	virtual bool IsConnected()
	{
		return true;
	}

	virtual int64 GetFrameData(
		int64 LastFrame,
		TArray<int32> &Triangles,
		TArray<FVector> &Vertices,
		TArray<FColor> &VertexColors,
		TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> &ColorBgra,
		int32 &ColorWidth,
		int32 &ColorHeight,
		TArray<uint8> &AudioData,
		int32 &Channels,
		int32 &SampleRate,
		TArray<FSK_Hand> &Hands,
		bool &HeadDetected,
		FVector &HeadPosition) = 0;
};
