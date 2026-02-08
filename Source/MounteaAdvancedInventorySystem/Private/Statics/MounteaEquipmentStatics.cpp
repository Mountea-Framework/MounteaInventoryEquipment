// REPLACE COPYRIGHT


#include "Statics/MounteaEquipmentStatics.h"

#include "Settings/MounteaAdvancedEquipmentSettingsConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

namespace EquipmentStatics
{
	static UMounteaAdvancedInventorySettings* GetInventorySettings()
	{
		return GetMutableDefault<UMounteaAdvancedInventorySettings>();
	}
}

UMounteaAdvancedEquipmentSettingsConfig* UMounteaEquipmentStatics::GetEquipmentSettingsConfig()
{
	return EquipmentStatics::GetInventorySettings() ? EquipmentStatics::GetInventorySettings()->AdvancedEquipmentSettingsConfig.LoadSynchronous() : nullptr;
}

TSet<TSoftClassPtr<USceneComponent>> UMounteaEquipmentStatics::GetAllowedAttachmentTargets()
{
	if (const auto* equipmentConfig = GetEquipmentSettingsConfig())
		return equipmentConfig->AllowedAttachmentTargets;

	TSet<TSoftClassPtr<USceneComponent>> returnValue;

	returnValue.Add(TSoftClassPtr<USceneComponent>(USceneComponent::StaticClass()));
	returnValue.Add(TSoftClassPtr<USceneComponent>(UStaticMeshComponent::StaticClass()));
	returnValue.Add(TSoftClassPtr<USceneComponent>(USkeletalMeshComponent::StaticClass()));

	return returnValue;
}

