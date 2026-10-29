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

#include "Statics/MounteaAdvancedInventoryItemTemplateEditorStatics.h"

#include "Helpers/MounteaInventoryImportHelpers.h"
#include "Helpers/MounteaInventoryZipHelper.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "UObject/SavePackage.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ObjectTools.h"

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFile(
    TArray<UMounteaInventoryItemTemplate*>& OutTemplates, 
    FString& OutErrorMessage)
{
    const FString filePath = ShowOpenFileDialog(
        TEXT("Import Templates"),
        TEXT("Mountea Item Files (*.mnteaitem;*.mnteaitems)|*.mnteaitem;*.mnteaitems")
    );

    if (filePath.IsEmpty())
    {
        OutErrorMessage = TEXT("No file selected");
        return false;
    }

    const FString folder = ShowContentBrowserPathPicker(TEXT("Select Item Import Folder"), TEXT("/Game/"));
	if (folder.IsEmpty())
	{
		OutErrorMessage = TEXT("No import folder selected");
		return false;
	}

	return ImportTemplatesFromFilePath(filePath, folder, OutTemplates, OutErrorMessage);
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(
    const FString& FilePath,
    const FString& TargetFolder,
    TArray<UMounteaInventoryItemTemplate*>& OutTemplates,
    FString& OutErrorMessage)
{
    OutTemplates.Reset();
    OutErrorMessage.Reset();
    TArray<TArray<uint8>> archives;
	if (!FMounteaInventoryZipHelper::ReadEntityArchives(FilePath, TEXT("mnteaitem"), TEXT("mnteaitems"), archives, OutErrorMessage))
		return false;

	TSet<FGuid> rootGuids;
    // Validate the whole selection before the first asset is changed.
    for (const TArray<uint8>& archive : archives)
    {
        FString json, taxonomy;
        TArray<FString> bundledItems;
		if (!FMounteaInventoryZipHelper::ExtractEntityArchive(archive, TEXT("item.json"), json, bundledItems, taxonomy, OutErrorMessage))
			return false;

		const FGuid guid = ExtractGuidFromJson(json);

		if (rootGuids.Contains(guid))
		{
			OutErrorMessage = TEXT("Duplicate item GUID in bundle");
			return false;
		}

		rootGuids.Add(guid);
        bundledItems.Add(json);

        for (const FString& itemJson : bundledItems)
        {
            UMounteaInventoryItemTemplate* transient = NewObject<UMounteaInventoryItemTemplate>();
			if (!ParseSingleTemplateJson(itemJson, transient, OutErrorMessage))
				return false;
		}
    }

    TArray<UMounteaInventoryItemTemplate*> imported;

    for (const TArray<uint8>& archive : archives)
		if (!ImportTemplatesFromZipBytes(archive, TargetFolder, FilePath, imported, OutErrorMessage))
			return false;

	OutTemplates.Append(imported);
    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ExportTemplatesToFile(
    const TArray<UMounteaInventoryItemTemplate*>& Templates, 
    FString& OutErrorMessage)
{
    if (Templates.Num() == 0 || !IsValid(Templates[0]))
    {
        OutErrorMessage = TEXT("No templates to export");
        return false;
    }

    const bool bMultipleTemplates = Templates.Num() > 1;
    const FString fileTypes = bMultipleTemplates 
        ? TEXT("Mountea Items Files (*.mnteaitems)|*.mnteaitems")
        : TEXT("Mountea Item Files (*.mnteaitem)|*.mnteaitem");
    
    FString defaultFileName = bMultipleTemplates 
        ? TEXT("ExportedTemplates") 
        : Templates[0]->DisplayName.ToString().Replace(TEXT(" "), TEXT("_"));

    const FString filePath = ShowSaveFileDialog(
        TEXT("Export Templates"),
        defaultFileName,
        fileTypes
    );

    if (filePath.IsEmpty())
    {
        OutErrorMessage = TEXT("No file selected");
        return false;
    }

    return ExportTemplatesToFilePath(Templates, filePath, OutErrorMessage);
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ExportTemplatesToFilePath(
    const TArray<UMounteaInventoryItemTemplate*>& Templates,
    const FString& FilePath,
    FString& OutErrorMessage)
{
    TSet<FGuid> guids;

    for (const auto* item : Templates)
    {
        if (!IsValid(item) || !item->Guid.IsValid() || guids.Contains(item->Guid))
		{
			OutErrorMessage = TEXT("Item export requires valid assets with distinct valid GUIDs");
			return false;
		}

		guids.Add(item->Guid);
    }

    TArray<FString> jsons;
	if (!FMounteaInventoryImportHelpers::SerializeBundledItems(Templates, jsons, OutErrorMessage))
		return false;

	TMap<FGuid, TArray<uint8>> archives;
    for (const FString& json : jsons)
    {
        TArray<uint8> bytes;
		if (!FMounteaInventoryZipHelper::BuildEntityArchive(TEXT("item.json"), json, {}, bytes, OutErrorMessage))
			return false;

		archives.Add(ExtractGuidFromJson(json), MoveTemp(bytes));
    }

    return FMounteaInventoryZipHelper::WriteEntityArchives(archives, FilePath, TEXT("mnteaitem"), TEXT("mnteaitems"), OutErrorMessage);
}

TArray<UMounteaInventoryItemTemplate*> UMounteaAdvancedInventoryItemTemplateEditorStatics::LoadAllExistingTemplates()
{
    TArray<UMounteaInventoryItemTemplate*> templates;
    
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> assetData;
    const FTopLevelAssetPath assetPath = UMounteaInventoryItemTemplate::StaticClass()->GetClassPathName();
    assetRegistryModule.Get().GetAssetsByClass(assetPath, assetData, true);

    for (const FAssetData& asset : assetData)
    {
        if (UMounteaInventoryItemTemplate* itemTemplate = Cast<UMounteaInventoryItemTemplate>(asset.GetAsset()))
            templates.Add(itemTemplate);
    }

    return templates;
}

UMounteaInventoryItemTemplate* UMounteaAdvancedInventoryItemTemplateEditorStatics::FindTemplateByGuid(
    const TArray<UMounteaInventoryItemTemplate*>& Templates, 
    const FGuid& Guid)
{
    if (!Guid.IsValid())
        return nullptr;

    UMounteaInventoryItemTemplate* const* foundTemplate = Templates.FindByPredicate(
        [&Guid](const UMounteaInventoryItemTemplate* Template)
    {
        return IsValid(Template) && Template->Guid == Guid;
    });

    return foundTemplate ? *foundTemplate : nullptr;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::UpdateExistingTemplate(
    UMounteaInventoryItemTemplate* Template,
    const FString& JsonString,
    const FString& SourceFilePath,
    FString& OutErrorMessage)
{
    if (!IsValid(Template))
    {
        OutErrorMessage = TEXT("Invalid template object for update");
        return false;
    }

    if (ExtractGuidFromJson(JsonString) != Template->Guid)
    {
        OutErrorMessage = TEXT("Imported item GUID does not match the existing template");
        return false;
    }

    if (!ParseSingleTemplateJson(JsonString, Template, OutErrorMessage))
        return false;

    Template->ReloadItemActions();
	if (!Template->CalculateJson())
	{
		OutErrorMessage = TEXT("Failed to serialize updated item");
		return false;
	}

	Template->SetSourceFilePath(SourceFilePath);
    
    UPackage* templatePackage = Template->GetOutermost();

    if (!templatePackage)
    {
        OutErrorMessage = TEXT("Template has no valid package");
        return false;
    }

    templatePackage->MarkPackageDirty();

    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    saveArgs.SaveFlags = SAVE_NoError;

    const FString packageFileName = FPackageName::LongPackageNameToFilename(
        templatePackage->GetName(), 
        FPackageName::GetAssetPackageExtension()
    );
    
    if (!UPackage::SavePackage(templatePackage, Template, *packageFileName, saveArgs))
    {
        OutErrorMessage = TEXT("Failed to save updated template");
        return false;
    }

    return true;
}

FGuid UMounteaAdvancedInventoryItemTemplateEditorStatics::ExtractGuidFromJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> jsonObject;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
        return FGuid();

    FString guidString;

    if (jsonObject->TryGetStringField(TEXT("guid"), guidString))
    {
        FGuid guid;

        if (FGuid::Parse(guidString, guid))
            return guid;
    }

    return FGuid();
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ExtractNameFromJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> jsonObject;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
        return TEXT("");

    FString displayName;

    if (jsonObject->TryGetStringField(TEXT("displayName"), displayName))
        return FMounteaInventoryImportHelpers::SanitizeAssetName(displayName);

    return TEXT("");
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseSingleTemplateJson(
    const FString& JsonString, 
    UMounteaInventoryItemTemplate* Template, 
    FString& OutErrorMessage)
{
    if (!IsValid(Template))
    {
        OutErrorMessage = TEXT("Invalid template object");
        return false;
    }

    TSharedPtr<FJsonObject> jsonObject;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
    {
        OutErrorMessage = TEXT("Failed to parse JSON");
        return false;
    }

    FString guidText;
    FGuid guid;

    if (!jsonObject->TryGetStringField(TEXT("guid"), guidText) || !FGuid::Parse(guidText, guid) || !guid.IsValid())
    {
        OutErrorMessage = TEXT("Missing or invalid item GUID");
        return false;
    }

    if (!DeserializeTemplateFromJson(jsonObject, Template, OutErrorMessage))
        return false;

    Template->SetJson(JsonString);

    return true;
}

UMounteaInventoryItemTemplate* UMounteaAdvancedInventoryItemTemplateEditorStatics::CreateTemplateAsset(
    const FString& TargetFolder, 
    const FString& AssetName, 
    FString& OutErrorMessage)
{
    FString safeName = ObjectTools::SanitizeObjectName(AssetName);
	if (safeName.IsEmpty())
		safeName = TEXT("Item");
	FString packageName, uniqueName;
    FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get().CreateUniqueAssetName(
        FPaths::Combine(TargetFolder, safeName), TEXT(""), packageName, uniqueName);
    UPackage* newPackage = CreatePackage(*packageName);
    
    if (!newPackage)
    {
        OutErrorMessage = FString::Printf(TEXT("Failed to create package: %s"), *packageName);
        return nullptr;
    }

    UMounteaInventoryItemTemplate* newTemplate = NewObject<UMounteaInventoryItemTemplate>(
        newPackage, 
        UMounteaInventoryItemTemplate::StaticClass(),
        *uniqueName,
        RF_Public | RF_Standalone
    );

    if (!newTemplate)
    {
        OutErrorMessage = TEXT("Failed to create template object");
        return nullptr;
    }

    return newTemplate;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::SaveTemplateAsset(
    UMounteaInventoryItemTemplate* Template,
    const FString& PackagePath)
{
    return FMounteaInventoryImportHelpers::SaveNewAssetPackage(Template, PackagePath);
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowOpenFileDialog(
    const FString& DialogTitle, 
    const FString& FileTypes)
{
    IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();

    if (!desktopPlatform)
        return FString();

    TArray<FString> openFilenames;
    const bool bFileSelected = desktopPlatform->OpenFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
        DialogTitle,
        FPaths::ProjectContentDir(),
        TEXT(""),
        FileTypes,
        EFileDialogFlags::None,
        openFilenames
    );

    return (bFileSelected && openFilenames.Num() > 0) ? openFilenames[0] : FString();
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowSaveFileDialog(
    const FString& DialogTitle, 
    const FString& DefaultFileName, 
    const FString& FileTypes)
{
    IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();

    if (!desktopPlatform)
   return FString();

    TArray<FString> saveFilenames;
    const bool bFileSelected = desktopPlatform->SaveFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
        DialogTitle,
        FPaths::ProjectContentDir(),
        DefaultFileName,
        FileTypes,
        EFileDialogFlags::None,
        saveFilenames
    );

    return (bFileSelected && saveFilenames.Num() > 0) ? saveFilenames[0] : FString();
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowFolderDialog(
    const FString& DialogTitle, 
    const FString& DefaultPath)
{
    IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();

    if (!desktopPlatform)
    {
        return FString();
    }

    FString selectedFolder;
    const bool bFolderSelected = desktopPlatform->OpenDirectoryDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
        DialogTitle,
        DefaultPath,
        selectedFolder
    );

    return bFolderSelected ? selectedFolder : FString();
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowContentBrowserPathPicker(
    const FString& DialogTitle, 
    const FString& DefaultPath)
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
    FString selectedPath;
    
    FPathPickerConfig pathPickerConfig;
    pathPickerConfig.DefaultPath = DefaultPath;
    pathPickerConfig.bAllowContextMenu = true;
    pathPickerConfig.bAllowClassesFolder = false;
    pathPickerConfig.OnPathSelected = FOnPathSelected::CreateLambda([&selectedPath](const FString& Path)
    {
        selectedPath = Path;
    });

	    TSharedRef<SWidget> pathPicker = contentBrowserModule.Get().CreatePathPicker(pathPickerConfig);
	    
	    TSharedPtr<SWindow> pickerWindow;
	    pickerWindow = SNew(SWindow)
	        .Title(FText::FromString(DialogTitle))
	        .ClientSize(FVector2D(450, 600))
	        .SupportsMaximize(false)
	        .SupportsMinimize(false)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                pathPicker
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNullWidget::NullWidget
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(5, 0)
	                [
	                    SNew(SButton)
	                    .Text(FText::FromString(TEXT("OK")))
	                    .OnClicked_Lambda([&pickerWindow]()
	                    {
	                        if (pickerWindow.IsValid())
                                pickerWindow->RequestDestroyWindow();
	                        return FReply::Handled();
	                    })
	                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
	                    SNew(SButton)
	                    .Text(FText::FromString(TEXT("Cancel")))
	                    .OnClicked_Lambda([&pickerWindow, &selectedPath]()
	                    {
	                        selectedPath.Empty();

	                        if (pickerWindow.IsValid())
                                pickerWindow->RequestDestroyWindow();
	                        return FReply::Handled();
	                    })
	                ]
	            ]
	        ];

	    if (pickerWindow.IsValid())
	        GEditor->EditorAddModalWindow(pickerWindow.ToSharedRef());
	    
	    return selectedPath;
	}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::DeserializeTemplateFromJson(
    const TSharedPtr<FJsonObject>& JsonObject, 
    UMounteaInventoryItemTemplate* Template, 
    FString& OutErrorMessage)
{
    if (!JsonObject.IsValid() || !IsValid(Template))
    {
        OutErrorMessage = TEXT("Invalid JSON object or template");
        return false;
    }

    FString guidString;

    if (JsonObject->TryGetStringField(TEXT("guid"), guidString))
    {
        FGuid parsedGuid;        

        if (FGuid::Parse(guidString, parsedGuid))
            Template->Guid = parsedGuid;
        else
            OutErrorMessage.Append(TEXT("\nParse GUID: Failed"));
    }

    FString displayName;

    if (JsonObject->TryGetStringField(TEXT("displayName"), displayName))
        Template->DisplayName = FText::FromString(displayName);

    JsonObject->TryGetStringField(TEXT("category"), Template->ItemCategory);
    JsonObject->TryGetStringField(TEXT("subCategory"), Template->ItemSubCategory);
    JsonObject->TryGetStringField(TEXT("rarity"), Template->ItemRarity);

    int32 itemFlags;

    if (JsonObject->TryGetNumberField(TEXT("flags"), itemFlags))
        Template->ItemFlags = static_cast<uint8>(itemFlags);

    JsonObject->TryGetNumberField(TEXT("maxQuantity"), Template->MaxQuantity);
    JsonObject->TryGetNumberField(TEXT("maxStackSize"), Template->MaxStackSize);

    DeserializeGameplayTagContainer(JsonObject, TEXT("tags"), Template->Tags);

    const TSharedPtr<FJsonObject>* spawnActorObject;

    if (JsonObject->TryGetObjectField(TEXT("spawnActor"), spawnActorObject))
    {
        FString spawnActorPath;

        if ((*spawnActorObject)->TryGetStringField(TEXT("path"), spawnActorPath) && !spawnActorPath.IsEmpty())
            Template->SpawnActor = TSoftClassPtr<AActor>(FSoftObjectPath(spawnActorPath));
    }

    const TSharedPtr<FJsonObject>* descriptionObject;

    if (JsonObject->TryGetObjectField(TEXT("description"), descriptionObject))
    {
        FString shortDesc, longDesc;

        if ((*descriptionObject)->TryGetStringField(TEXT("short"), shortDesc))
            Template->ItemShortInfo = FText::FromString(shortDesc);
        if ((*descriptionObject)->TryGetStringField(TEXT("long"), longDesc))
            Template->ItemLongInfo = FText::FromString(longDesc);
    }

    const TSharedPtr<FJsonObject>* visualsObject;

    if (JsonObject->TryGetObjectField(TEXT("visuals"), visualsObject))
    {
        const TSharedPtr<FJsonObject>* thumbnailObject;

        if ((*visualsObject)->TryGetObjectField(TEXT("thumbnail"), thumbnailObject))
        {
            FString thumbnailPath;

            if ((*thumbnailObject)->TryGetStringField(TEXT("path"), thumbnailPath) && !thumbnailPath.IsEmpty())
                Template->ItemThumbnail = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(thumbnailPath));
        }

        const TSharedPtr<FJsonObject>* coverObject;

        if ((*visualsObject)->TryGetObjectField(TEXT("cover"), coverObject))
        {
            FString coverPath;

            if ((*coverObject)->TryGetStringField(TEXT("path"), coverPath) && !coverPath.IsEmpty())
                Template->ItemCover = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(coverPath));
        }

        const TSharedPtr<FJsonObject>* meshObject;

        if ((*visualsObject)->TryGetObjectField(TEXT("mesh"), meshObject))
        {
            FString meshPath;

            if ((*meshObject)->TryGetStringField(TEXT("path"), meshPath) && !meshPath.IsEmpty())
            {
                FSoftObjectPath softPath(meshPath);

                if (UObject* loadedMesh = softPath.TryLoad())
                {
                    if (UStreamableRenderAsset* renderAsset = Cast<UStreamableRenderAsset>(loadedMesh))
                        Template->ItemMesh = renderAsset;
                }
            }
        }
    }

    const TSharedPtr<FJsonObject>* durabilityObject;

    if (JsonObject->TryGetObjectField(TEXT("durability"), durabilityObject))
    {
        (*durabilityObject)->TryGetBoolField(TEXT("enabled"), Template->bHasDurability);
        
        if (Template->bHasDurability)
        {
            double maxDurability, baseDurability, penalization, priceCoef;

            if ((*durabilityObject)->TryGetNumberField(TEXT("max"), maxDurability))
                Template->MaxDurability = static_cast<float>(maxDurability);
            if ((*durabilityObject)->TryGetNumberField(TEXT("base"), baseDurability))
                Template->BaseDurability = static_cast<float>(baseDurability);
            if ((*durabilityObject)->TryGetNumberField(TEXT("penalization"), penalization))
                Template->DurabilityPenalization = static_cast<float>(penalization);
            if ((*durabilityObject)->TryGetNumberField(TEXT("priceCoefficient"), priceCoef))
                Template->DurabilityToPriceCoefficient = static_cast<float>(priceCoef);
        }
    }

    const TSharedPtr<FJsonObject>* economyObject;

    if (JsonObject->TryGetObjectField(TEXT("economy"), economyObject))
    {
        (*economyObject)->TryGetBoolField(TEXT("enabled"), Template->bHasPrice);
        
        if (Template->bHasPrice)
        {
            double basePrice, sellCoef;

            if ((*economyObject)->TryGetNumberField(TEXT("basePrice"), basePrice))
                Template->BasePrice = static_cast<float>(basePrice);
            if ((*economyObject)->TryGetNumberField(TEXT("sellCoefficient"), sellCoef))
                Template->SellPriceCoefficient = static_cast<float>(sellCoef);
        }
    }

    const TSharedPtr<FJsonObject>* weightObject;

    if (JsonObject->TryGetObjectField(TEXT("weight"), weightObject))
    {
        (*weightObject)->TryGetBoolField(TEXT("enabled"), Template->bHasWeight);
        
        if (Template->bHasWeight)
        {
            double weightValue;

            if ((*weightObject)->TryGetNumberField(TEXT("value"), weightValue))
                Template->Weight = static_cast<float>(weightValue);
        }
    }

    DeserializeGameplayTagContainer(JsonObject, TEXT("attachmentSlots"), Template->AttachmentSlots);
    DeserializeSoftClassPtrSet(JsonObject, TEXT("specialAffects"), Template->ItemSpecialAffects);

    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::DeserializeGameplayTagContainer(
    const TSharedPtr<FJsonObject>& JsonObject, 
    const FString& FieldName, 
    FGameplayTagContainer& OutContainer)
{
    OutContainer.Reset();
    
    const TArray<TSharedPtr<FJsonValue>>* tagsArray;

    if (!JsonObject->TryGetArrayField(FieldName, tagsArray))
        return false;

    for (const auto& tagValue : *tagsArray)
    {
        FString tagString;

        if (tagValue->TryGetString(tagString))
        {
            const FGameplayTag tag = FGameplayTag::RequestGameplayTag(FName(*tagString), false);

            if (tag.IsValid())
                OutContainer.AddTag(tag);
        }
    }

    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseItemJsonFromZip(
    const TArray<uint8>& ZipBytes,
    FString& OutItemJson,
    FString& OutErrorMessage)
{
    TArray<FString> bundledItems;
    FString taxonomy;
    return FMounteaInventoryZipHelper::ExtractEntityArchive(ZipBytes, TEXT("item.json"), OutItemJson,
        bundledItems, taxonomy, OutErrorMessage);
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromZipBytes(
    const TArray<uint8>& ZipBytes,
    const FString& TargetFolder,
    const FString& SourceFilePath,
    TArray<UMounteaInventoryItemTemplate*>& OutTemplates,
    FString& OutErrorMessage)
{
    FString itemJson, taxonomy;
    TArray<FString> bundledItems;
	if (!FMounteaInventoryZipHelper::ExtractEntityArchive(ZipBytes, TEXT("item.json"), itemJson, bundledItems, taxonomy, OutErrorMessage))
		return false;

	TArray<FString> allJsons = bundledItems;
    allJsons.Add(itemJson);

    for (const FString& json : allJsons)
    {
        UMounteaInventoryItemTemplate* transient = NewObject<UMounteaInventoryItemTemplate>();
		if (!ParseSingleTemplateJson(json, transient, OutErrorMessage))
			return false;
	}
    const FGuid itemGuid = ExtractGuidFromJson(itemJson);
    const TArray<UMounteaInventoryItemTemplate*> existingTemplates = LoadAllExistingTemplates();
    UMounteaInventoryItemTemplate* existing = FindTemplateByGuid(existingTemplates, itemGuid);
    const FString folderToUse = TargetFolder.IsEmpty() ? TEXT("/Game/") : TargetFolder;
    TMap<FGuid, UMounteaInventoryItemTemplate*> itemsByGuid = FMounteaInventoryImportHelpers::BuildProjectItemGuidMap();
	if (!FMounteaInventoryImportHelpers::ImportBundledItems(bundledItems, folderToUse, SourceFilePath, itemsByGuid, OutErrorMessage))
		return false;

	if (existing)
    {
		if (!UpdateExistingTemplate(existing, itemJson, SourceFilePath, OutErrorMessage))
			return false;

		OutTemplates.Add(existing);
        return true;
    }

    const FString displayName = ExtractNameFromJson(itemJson);
    const FString assetName = displayName.IsEmpty() ? itemGuid.ToString(EGuidFormats::DigitsWithHyphens) : displayName;
    UMounteaInventoryItemTemplate* item = CreateTemplateAsset(folderToUse, assetName, OutErrorMessage);
	if (!item || !ParseSingleTemplateJson(itemJson, item, OutErrorMessage))
		return false;

	item->ReloadItemActions();

	if (!item->CalculateJson())
	{
		OutErrorMessage = TEXT("Failed to serialize imported item");
		return false;
	}

	item->SetSourceFilePath(SourceFilePath);

	if (!SaveTemplateAsset(item, item->GetOutermost()->GetName()))
	{
		OutErrorMessage = TEXT("Failed to save imported item");
		return false;
	}

	OutTemplates.Add(item);
    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::DeserializeSoftClassPtrSet(
    const TSharedPtr<FJsonObject>& JsonObject, 
    const FString& FieldName, 
    TSet<TSoftClassPtr<UObject>>& OutSet)
{
    OutSet.Empty();
    
    const TArray<TSharedPtr<FJsonValue>>* classArray;

    if (!JsonObject->TryGetArrayField(FieldName, classArray))
        return false;

    for (const auto& classValue : *classArray)
    {
        FString classPath;

        if (classValue->TryGetString(classPath) && !classPath.IsEmpty())
        {
            TSoftClassPtr<UObject> softClass = TSoftClassPtr<UObject>(FSoftObjectPath(classPath));
            OutSet.Add(softClass);
        }
    }

    return true;
}
