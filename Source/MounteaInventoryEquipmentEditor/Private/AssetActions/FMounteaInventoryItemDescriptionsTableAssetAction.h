// All rights reserved Dominik Morse 2023.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_CSVAssetBase.h"
#include "Utils/MounteaInventoryEquipmentEditorUtilities.h"

class FMounteaInventoryItemDescsAssetAction : public FAssetTypeActions_CSVAssetBase
{
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	
	virtual const TArray<FText>& GetSubMenus() const override;
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;

protected:

	/** Handler for when JSON is selected */
	void ExecuteExportAsJSON(TArray< TWeakObjectPtr<UObject> > Objects);

private:

	/** Handler for opening the source file for this asset */
	void ProcessFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions);

	/** Determine whether the find source file in explorer editor command can execute or not */
	bool CanProcessFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions) const;

	/** Verify the specified filename exists */
	bool CheckFileExists(const FString& InFileName) const;
	
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
