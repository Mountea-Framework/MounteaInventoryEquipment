// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "MounteaEditorDefaultsSubsystem.generated.h"

/**
 * Seeds missing Mountea inventory settings defaults at editor startup.
 * This subsystem is editor-only and does not affect packaged runtime builds.
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaEditorDefaultsSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

	void ApplyDefaultsIfNeeded() const;

	template <typename TObjectType>
	bool TryAssignMissing(TSoftObjectPtr<TObjectType>& TargetProperty, const TCHAR* ObjectPath, const TCHAR* PropertyName) const;

private:

	static constexpr const TCHAR* InputMappingPath =
		TEXT("/MounteaAdvancedInventorySystem/Input/IMC_Inventory.IMC_Inventory");
	static constexpr const TCHAR* InventorySettingsConfigPath =
		TEXT("/MounteaAdvancedInventorySystem/Data/Config/DefaultInventoryConfig.DefaultInventoryConfig");
	static constexpr const TCHAR* EquipmentSettingsConfigPath =
		TEXT("/MounteaAdvancedInventorySystem/Data/Config/DefaultEquipmentConfig.DefaultEquipmentConfig");
	static constexpr const TCHAR* InventoryUISettingsConfigPath =
		TEXT("/MounteaAdvancedInventorySystem/Data/Config/DefaultInventoryUIConfig.DefaultInventoryUIConfig");
	static constexpr const TCHAR* CraftingSettingsConfigPath =
		TEXT("/MounteaAdvancedInventorySystem/Data/Config/DefaultMounteaCraftingConfig.DefaultMounteaCraftingConfig");
	static constexpr const TCHAR* CraftingUISettingsConfigPath =
		TEXT("/MounteaAdvancedInventorySystem/Data/Config/DefaultCraftingUISettingsConfig.DefaultCraftingUISettingsConfig");
};

template <typename TObjectType>
bool UMounteaEditorDefaultsSubsystem::TryAssignMissing(
	TSoftObjectPtr<TObjectType>& TargetProperty,
	const TCHAR* ObjectPath,
	const TCHAR* PropertyName
) const
{
	if (!TargetProperty.ToSoftObjectPath().IsNull())
		return false;

	const FSoftObjectPath softObjectPath(ObjectPath);
	UObject* resolvedObject = softObjectPath.ResolveObject();
	if (!resolvedObject)
		resolvedObject = softObjectPath.TryLoad();

	if (!resolvedObject)
	{
		LOG_WARNING(
			TEXT("[Editor Defaults] Missing default asset for '%s': %s"),
			PropertyName,
			ObjectPath
		);
		return false;
	}

	if (!Cast<TObjectType>(resolvedObject))
	{
		LOG_WARNING(
			TEXT("[Editor Defaults] Asset type mismatch for '%s': %s"),
			PropertyName,
			ObjectPath
		);
		return false;
	}

	TargetProperty = TSoftObjectPtr<TObjectType>(softObjectPath);
	LOG_INFO(
		TEXT("[Editor Defaults] Applied default '%s' from %s"),
		PropertyName,
		ObjectPath
	);
	return true;
}
