// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

class UMounteaInventoryItemBase;

class FMounteaInventoryItemAssetAction : public FAssetTypeActions_Base
{
	
public:

	FMounteaInventoryItemAssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions ( const TArray<UObject*>& InObjects ) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section) override;
	virtual bool CanFilter() override	{ return false; };
	
	virtual const TArray<FText>& GetSubMenus() const override;
	
	virtual void BuildBackendFilter(FARFilter& InFilter) override
	{
		FilterAddNativeParentClassPath(InFilter, GetSupportedClass());

		// Add to filter all native children classes of our supported class
		TArray<UClass*> NativeChildClasses;
		TArray<UClass*> BlueprintChildClasses;
		FMounteaInventoryEquipmentEditorUtilities::GetAllChildClassesOf(GetSupportedClass(), NativeChildClasses, BlueprintChildClasses);
		for (const UClass* ChildNativeClass : NativeChildClasses)
		{
			FilterAddNativeParentClassPath(InFilter, ChildNativeClass);
		}

		InFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
		InFilter.bRecursiveClasses = true;
	};
	static void FilterAddNativeParentClassPath(FARFilter& InFilter, const UClass* Class)
	{
		if (Class == nullptr)
		{
			return;
		}

		const FString Value = FString::Printf(
			TEXT("%s'%s'"),
			*UClass::StaticClass()->GetName(),
			*Class->GetPathName()
		);
		InFilter.TagsAndValues.Add(FBlueprintTags::NativeParentClassPath, Value);
	};

private:
	
	void ExecuteRefreshItem(TArray<TWeakObjectPtr<UMounteaInventoryItemBase>> Items);
};

class FAssetActionExtender_MounteaInventoryItem
{
public:
	static void RegisterMenus();
	static void ExecuteRefreshItem(const struct FToolMenuContext& MenuContext);
};