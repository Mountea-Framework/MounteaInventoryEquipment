// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaEquipmentHelpers.h"
#include "MounteaEquipmentConfigData.generated.h"


/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Mountea Equipment Data")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaEquipmentConfigData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "1. Required", meta=(AllowAbstract=false, NoResetToDefault, NoElementDuplicate))
	TSet<FMounteaEquipmentSlotIdentity> EquipmentSlotIDs;

#if WITH_EDITOR

	virtual void PreEditChange(FEditPropertyChain& PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
	bool ValidateConfig(TArray<FText>& Notifications, TSet<FMounteaEquipmentSlotIdentity>& DirtySlots);

	DECLARE_DELEGATE(FConfigModified);
	FConfigModified OnConfigModified;

	DECLARE_DELEGATE_OneParam(FPropertyModified, FProperty*);
	FPropertyModified OnPropertyModified;

	DECLARE_DELEGATE_OneParam(FTagPropertyModified, FGameplayTag&);
	FTagPropertyModified OnTagModified;
	
#endif
	
};
