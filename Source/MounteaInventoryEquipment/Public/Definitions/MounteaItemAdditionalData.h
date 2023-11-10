// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "Helpers/MounteaInventoryHelpers.h"


#include "MounteaItemAdditionalData.generated.h"

class UMounteaInventoryItemsDescriptionsTable;

#define LOCTEXT_NAMESPACE "MounteaItemAdditionalData"

/**
 * Additional data for a Mountea inventory item.
 *
 * This class represents additional data associated with a Mountea inventory item. It serves as a data asset
 * that can be assigned to an inventory item to provide extra information or functionality specific to that item.
 * The additional data can be customized and configured in the editor, allowing designers to define unique
 * properties or behaviors for each item.
 *
 * @see UDataAsset
 * @see UMounteaInventoryItemBase
 * @see https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki/Inventory-Item-Additional-Data
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Additional Data")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaItemAdditionalData : public UDataAsset
{
	GENERATED_BODY()

	UMounteaItemAdditionalData();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(DisplayThumbnail=false, DisplayName="Long Description (Source)"))
	UMounteaInventoryItemsDescriptionsTable* SourceTable_LongDescription = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(GetOptions="GetSourceTableRows_LongDescription", EditCondition="SourceTable_LongDescription!=nullptr", DisplayName="Long Description (Row)"))
	FName SourceRow_LongDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(ShowOnlyInnerProperties, MultiLine=true, EditCondition="SourceTable_LongDescription==nullptr"))
	FText ItemLongDescription;

		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(DisplayThumbnail=false, DisplayName="Short Description (Source)"))
	UMounteaInventoryItemsDescriptionsTable* SourceTable_ShortDescription = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(GetOptions="GetSourceTableRows_ShortDescription", EditCondition="SourceTable_ShortDescription!=nullptr", DisplayName="Short Description (Row)"))
	FName SourceRow_ShortDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true, EditCondition="SourceTable_ShortDescription==nullptr"))
	FText ItemShortDescription;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip", meta=(ShowOnlyInnerProperties))
	UTexture2D* ItemThumbnail = nullptr;

private:
	
	static FMounteaItemDescription* GetRowData(const UMounteaInventoryItemsDescriptionsTable* FromTable, FName FromRow);

	UFUNCTION()
	TArray<FName> GetSourceTableRows_ShortDescription() const;

	UFUNCTION()
	TArray<FName> GetSourceTableRows_LongDescription() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};

#undef LOCTEXT_NAMESPACE

