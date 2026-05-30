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
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryGlobalUIConfig.generated.h"

/**
 * UMounteaAdvancedInventoryGlobalUIConfig manages global UI configuration for 
 * the Mountea Advanced Inventory & Equipment System.
 *
 * @see [UI Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 * @see UMounteaAdvancedInventorySettings
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Global UI Config",
	meta=(ShortTooltip="Configuration asset for the Mountea Inventory & Equipment System UI."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryGlobalUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault))
	TSet<FString> ModalTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault),
		meta=(ShowOnlyInnerProperties),
		meta=(GetKeyOptions="GetModalTypes"))
	TMap<FString,TSoftClassPtr<UUserWidget>> Modals;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modals",
		meta=(NoResetToDefault),
		meta=(RequiredAssetDataTags = "RowStructure=/Script/MounteaAdvancedInventorySystem.MounteaModalsData"))
	TSet<TSoftObjectPtr<UDataTable>> ModalsData;
	
protected:

	UFUNCTION()
	FORCEINLINE TArray<FString> GetModalTypes() const
	{
		TArray<FString> returnValues;
		returnValues.Reserve(ModalTypes.Num() + 1);
		returnValues.Add(TEXT("none"));
		returnValues.Append(ModalTypes.Array());
		return returnValues;
	};
};
