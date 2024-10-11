// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaInventoryItemsLibrary.generated.h"

class UMounteaInventoryItemBlueprint;

/**
 * Represents a row in the Mountea Inventory Items Library.
 *
 * Holds information about an inventory item blueprint and its initial quantity.
 * Can be used as a key in TSet and TMap collections.
 *
 * @see UMounteaInventoryItemBlueprint
 */
USTRUCT(BlueprintType)
struct FMounteaInventoryItemsLibraryRow
{
	GENERATED_BODY()
	
	FMounteaInventoryItemsLibraryRow() : InitialQuantity(0), InventoryItemBlueprint(nullptr) {}

	explicit FMounteaInventoryItemsLibraryRow(UMounteaInventoryItemBlueprint* NewItem, const int32 Quantity = 1);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault), meta=(NoResetToDefault, UIMin=0,ClampMin=0))
	int32 InitialQuantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault))
	TObjectPtr<UMounteaInventoryItemBlueprint> InventoryItemBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault, EditCondition="InitialQuantity<0",EditConditionHides))
	FString InventoryItemBlueprintName = TEXT("");

public:

	void UpdateDisplayName();

public:

	bool operator==(const UMounteaInventoryItemBlueprint* ItemBlueprint) const;
	bool operator==(const FMounteaInventoryItemsLibraryRow& Other) const;
	bool operator!=(const UMounteaInventoryItemBlueprint* ItemBlueprint) const;
	bool operator!=(const FMounteaInventoryItemsLibraryRow& Other) const;

	friend uint32 GetTypeHash(const FMounteaInventoryItemsLibraryRow& Row);
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup="Mountea", DisplayName="Inventory Item Blueprints Library")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemsLibrary : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	TArray<UMounteaInventoryItemBlueprint*> GetInventoryItemBlueprints() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory&Equipment|ItemBlueprintsLibrary", meta=(CustomTag="MounteaK2Getter"))
	TArray<FMounteaInventoryItemsLibraryRow> GetInventoryItemBlueprintRows() const;
	
protected:
	
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category="Required", meta=(NoResetToDefault, UIMin=0,ClampMin=0, TitleProperty="{InitialQuantity} x {InventoryItemBlueprintName}"))
	TArray<FMounteaInventoryItemsLibraryRow> InventoryItemBlueprints;


#if WITH_EDITOR

	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	
#endif
	
};
