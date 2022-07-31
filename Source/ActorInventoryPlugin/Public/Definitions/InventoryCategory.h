// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/InventoryHelpers.h"
#include "InventoryCategory.generated.h"

class UInventoryKeyAction;

#define LOCTEXT_NAMESPACE "InventoryCategory"

/**
 * Inventory Category Data.
 * Defines name and basic attributes of all Inventory Items which are bound to this Category.
 *
 * Contains Set of Key Actions, Actions that are specific to this Category, are used for Inventory Items. Use or Drop, for Example.
 *
 * @see https://github.com/Mountea-Framework/ActorInventoryPlugin/wiki/Inventory-Category
 */
UCLASS(Blueprintable)
class ACTORINVENTORYPLUGIN_API UInventoryCategory : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FText GetCategoryName() const
	{
		return CategoryData.CategoryName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UTexture2D* GetCategoryIcon() const
	{
		return CategoryData.CategoryTexture;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE UInventoryCategory* GetParentCategory() const
	{
		return CategoryData.ParentCategory;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE int32 GetMaxQuantityPerStack() const
	{
		return CategoryData.MaxQuantityPerStack;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FGuid GetCategoryGUID() const
	{
		return Guid;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE FInventoryCategoryData GetCategoryData() const
	{ 
		return CategoryData;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<TSubclassOf<UInventoryKeyAction>> GetCategoryKeyActions() const { return CategoryKeyActions; };
	
protected:

	// Category Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn=true, ShowOnlyInnerProperties))
	FInventoryCategoryData CategoryData;

	// Unique GUID identifier for this record
	UPROPERTY(VisibleAnywhere, Category="Inventory|GUID")
	FGuid Guid = FGuid::NewGuid();

	// List of Allowed Category Actions that can be Executed for all Items of this Category.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowAbstract=false, NoElementDuplicate=true))
	TArray<TSubclassOf<UInventoryKeyAction>> CategoryKeyActions;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};

#undef LOCTEXT_NAMESPACE