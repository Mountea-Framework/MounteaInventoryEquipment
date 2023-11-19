// All rights reserved Dominik Morse 2023.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

class FMounteaInventoryContentThemeAssetAction : public FAssetTypeActions_Base
{
	
public:

	FMounteaInventoryContentThemeAssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;

	virtual const TArray<FText>& GetSubMenus() const override;

	virtual bool CanFilter() override
	{ return true; };
	
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
};

class FTextThemeAssetActions : public FMounteaInventoryContentThemeAssetAction
{
public:
	
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
};

class FCategoryThemeAssetActions : public FMounteaInventoryContentThemeAssetAction
{
public:
	
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
};