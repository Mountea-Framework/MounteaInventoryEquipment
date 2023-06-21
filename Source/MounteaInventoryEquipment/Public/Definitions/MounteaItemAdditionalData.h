// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Helpers/MounteaInventoryHelpers.h"
#include "Helpers/FMounteaTemplatesLibrary.h"

#include "MounteaItemAdditionalData.generated.h"

#define LOCTEXT_NAMESPACE "MounteaItemAdditionalData"

USTRUCT(BlueprintType)
struct FMounteaItemDescription : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true))
	FText ItemShortDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true))
	FText ItemLongDescription;
};

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Additional Data")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaItemAdditionalData : public UDataAsset
{
	GENERATED_BODY()

	UMounteaItemAdditionalData();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(DisplayThumbnail=false, DisplayName="Long Description (Source)"))
	UDataTable* SourceTable_LongDescription = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(GetOptions="GetSourceTableRows_LongDescription", EditCondition="SourceTable_LongDescription!=nullptr", DisplayName="Long Description (Row)"))
	FName SourceRow_LongDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Long Description", meta=(ShowOnlyInnerProperties, MultiLine=true, EditCondition="SourceTable_LongDescription==nullptr"))
	FText ItemLongDescription;

		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(DisplayThumbnail=false, DisplayName="Short Description (Source)"))
	UDataTable* SourceTable_ShortDescription = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(GetOptions="GetSourceTableRows_ShortDescription", EditCondition="SourceTable_ShortDescription!=nullptr", DisplayName="Short Description (Row)"))
	FName SourceRow_ShortDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Short Description", meta=(MultiLine=true, EditCondition="SourceTable_ShortDescription==nullptr"))
	FText ItemShortDescription;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tooltip", meta=(ShowOnlyInnerProperties))
	UTexture2D* ItemThumbnail = nullptr;

private:
	
	static FMounteaItemDescription* GetRowData(const UDataTable* FromTable, FName FromRow);

	UFUNCTION()
	TArray<FName> GetSourceTableRows_ShortDescription() const
	{
		return GetSourceRows<FName, UDataTable>(SourceTable_ShortDescription);
	}

	UFUNCTION()
	TArray<FName> GetSourceTableRows_LongDescription() const
	{
		return GetSourceRows<FName, UDataTable>(SourceTable_LongDescription);
	}

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};

#undef LOCTEXT_NAMESPACE

