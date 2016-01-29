#pragma once

#include "DepthSenseSoundWaveFactoryNew.generated.h"


/**
 * Implements a factory for UDepthSenseSoundWave objects.
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UDepthSenseSoundWaveFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()

	/** An initial UMediaPlayer asset to place in the newly created sound wave. */
	UPROPERTY()
	class UMediaPlayer* InitialMediaPlayer;

public:

	// UFactory Interface

	virtual UObject* FactoryCreateNew( UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn ) override;
	virtual bool ShouldShowInNewMenu() const override;
};
