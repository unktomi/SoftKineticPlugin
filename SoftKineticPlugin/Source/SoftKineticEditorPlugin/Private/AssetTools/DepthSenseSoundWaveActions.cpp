#include "SoftKineticEditorPluginPrivatePCH.h"



#define LOCTEXT_NAMESPACE "AssetTypeActions"


/* FAssetTypeActions_SoundBase overrides
 *****************************************************************************/

bool FDepthSenseSoundWaveActions::CanFilter()
{
	return true;
}


uint32 FDepthSenseSoundWaveActions::GetCategories()
{
	return EAssetTypeCategories::Sounds;
}


FText FDepthSenseSoundWaveActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DepthSenseSoundWave", "DepthSense Sound Wave");
}


UClass* FDepthSenseSoundWaveActions::GetSupportedClass() const
{
	return UDepthSenseSoundWave::StaticClass();
}


FColor FDepthSenseSoundWaveActions::GetTypeColor() const
{
	return FColor(77, 93, 239);
}


bool FDepthSenseSoundWaveActions::HasActions ( const TArray<UObject*>& InObjects ) const
{
	return false;
}


#undef LOCTEXT_NAMESPACE
