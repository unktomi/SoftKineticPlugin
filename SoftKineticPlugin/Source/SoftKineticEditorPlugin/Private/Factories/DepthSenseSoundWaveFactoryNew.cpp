#include "SoftKineticEditorPluginPrivatePCH.h"


/* UDepthSenseSoundWaveFactoryNew structors
 *****************************************************************************/

UDepthSenseSoundWaveFactoryNew::UDepthSenseSoundWaveFactoryNew( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	SupportedClass = UDepthSenseSoundWave::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UDepthSenseSoundWaveFactoryNew::FactoryCreateNew( UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn )
{
	UDepthSenseSoundWave* DepthSenseSoundWave = NewObject<UDepthSenseSoundWave>(InParent, InClass, InName, Flags);

	return DepthSenseSoundWave;
}


bool UDepthSenseSoundWaveFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
