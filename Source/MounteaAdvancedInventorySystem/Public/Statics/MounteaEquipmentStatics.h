// REPLACE COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaEquipmentStatics.generated.h"

class UMounteaAdvancedEquipmentSettingsConfig;


/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaEquipmentStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
#pragma region Settings

	/**
	 * Retrieves the advanced configuration settings for the Mountea Equipment System.
	 * Facilitates access to equipment-specific configurations like equipment slots, allowed attachment targets etc..
	 *
	 * @return - A pointer to the Mountea Advanced Equipment Settings configuration object
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Equipment Settings Config")
	static UMounteaAdvancedEquipmentSettingsConfig* GetEquipmentSettingsConfig();

	/**
	 * Retrieves the set of scene component classes that are permitted as attachment targets
	 * for the Mountea Equipment System. This information is defined in the advanced equipment
	 * settings configuration and determines which components can be valid attachment targets
	 * for equippable items.
	 *
	 * If no configuration is found, a default set of allowed attachment targets including
	 * USceneComponent, UStaticMeshComponent, and USkeletalMeshComponent will be returned.
	 *
	 * @return - A set of soft class pointers representing allowed attachment target classes for equipment.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory & Equipment|Config", 
		meta=(CustomTag="MounteaK2Getter"),
		DisplayName="Get Equipment Settings Config")
	static TSet<TSoftClassPtr<USceneComponent>> GetAllowedAttachmentTargets();
	
#pragma endregion
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory & Equipment|Equipment",
		meta=(CustomTag="MounteaK2Setter"),
		DisplayName="Equip Item")
	static bool EquipItem(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor);

#if WITH_EDITOR
	UFUNCTION(BlueprintInternalUseOnly)
	bool Prototype_EquipItem(const FInventoryItem& ItemDefinition, AActor*& OutSpawnedActor) { return false; };
#endif
	
};
