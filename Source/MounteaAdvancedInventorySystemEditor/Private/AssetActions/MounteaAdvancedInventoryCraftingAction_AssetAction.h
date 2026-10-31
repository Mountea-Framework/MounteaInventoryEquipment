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
#include "AssetTypeActions_Base.h"
#include "Utilities/MounteaAdvancedInventoryEditorUtilities.h"

/**
 * Asset actions for Mountea Crafting Action Blueprints.
 */
class FMounteaAdvancedInventoryCraftingAction_AssetAction : public FAssetTypeActions_Base
{
public:

	FMounteaAdvancedInventoryCraftingAction_AssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;

	virtual bool CanFilter() override
	{
		return false;
	}

	virtual void BuildBackendFilter(FARFilter& InFilter) override
	{
		FilterAddNativeParentClassPath(InFilter, GetSupportedClass());

		// Add all native child classes of the supported class to the filter
		TArray<UClass*> nativeChildClasses;
		TArray<UClass*> blueprintChildClasses;
		FMounteaAdvancedInventoryEditorUtilities::GetAllChildClassesOf(GetSupportedClass(), nativeChildClasses, blueprintChildClasses);
		for (const UClass* childNativeClass : nativeChildClasses)
		{
			FilterAddNativeParentClassPath(InFilter, childNativeClass);
		}

		InFilter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
		InFilter.bRecursiveClasses = true;
	}

	static void FilterAddNativeParentClassPath(FARFilter& InFilter, const UClass* Class)
	{
		if (Class == nullptr)
		{
			return;
		}

		const FString value = FString::Printf(
			TEXT("%s'%s'"),
			*UClass::StaticClass()->GetName(),
			*Class->GetPathName()
		);
		InFilter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, value);
	}
};
