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

#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "IContentBrowserSingleton.h"
#include "IDesktopPlatform.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "UObject/SavePackage.h"

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

    return ImportTemplatesFromFilePath(filePath, FString(), OutTemplates, OutErrorMessage);
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ImportTemplatesFromFilePath(
    const FString& FilePath,
    const FString& TargetFolder,
    TArray<UMounteaInventoryItemTemplate*>& OutTemplates, 
    FString& OutErrorMessage)
{
    FString fileContent;
    if (!FFileHelper::LoadFileToString(fileContent, *FilePath))
    {
        OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
        return false;
    }

    const bool bIsMultipleFile = FilePath.EndsWith(TEXT(".mnteaitems"));
    
    TArray<FString> itemJsons;
    if (bIsMultipleFile)
    {
        if (!ParseMultipleTemplatesJson(fileContent, itemJsons, OutErrorMessage))
            return false;
    }
    else
        itemJsons.Add(fileContent);

    TArray<UMounteaInventoryItemTemplate*> existingTemplates = LoadAllExistingTemplates();
    
    int32 updatedCount = 0;
    int32 createdCount = 0;

    for (const FString& itemJson : itemJsons)
    {
        const FGuid itemGuid = ExtractGuidFromJson(itemJson);

        if (UMounteaInventoryItemTemplate* TargetTemplate = FindTemplateByGuid(existingTemplates, itemGuid))
        {
            if (UpdateExistingTemplate(TargetTemplate, itemJson, OutErrorMessage))
            {
                OutTemplates.Add(TargetTemplate);
                updatedCount++;
            }
        }
        else
        {
            FString folderToUse = TargetFolder;
            if (folderToUse.IsEmpty())
            {
                folderToUse = ShowContentBrowserPathPicker(
                    NSLOCTEXT("UMounteaAdvancedInventoryItemTemplateEditorStatics", "Import_TargetFolder", 
                        "Select Target Folder for New Template").ToString(),
                    TEXT("/Game/")
                );

                if (folderToUse.IsEmpty())
                {
                    OutErrorMessage = TEXT("No target folder selected for new template");
                    continue;
                }
            }

            const FString itemDisplayName = ExtractNameFromJson(itemJson);
            const FString assetName = FString::Printf(TEXT("ImportedTemplate_%s"),  itemDisplayName.IsEmpty() ?
                *itemGuid.ToString(EGuidFormats::Short) : *itemDisplayName);
            UMounteaInventoryItemTemplate* newTemplate = CreateTemplateAsset(folderToUse, assetName, OutErrorMessage);
            
            if (!newTemplate)
                continue;

            if (ParseSingleTemplateJson(itemJson, newTemplate, OutErrorMessage))
            {
                newTemplate->CalculateJson();
                
                const FString packagePath = FPaths::Combine(folderToUse, assetName);
                if (SaveTemplateAsset(newTemplate, packagePath))
                {
                    OutTemplates.Add(newTemplate);
                    createdCount++;
                }
            }
        }
    }

    if (OutTemplates.Num() == 0)
    {
        OutErrorMessage = TEXT("No templates were successfully imported");
        return false;
    }

    OutErrorMessage = FString::Printf(
        TEXT("Import complete: %d created, %d updated"),
        createdCount,
        updatedCount
    );

    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ExportTemplatesToFile(
    const TArray<UMounteaInventoryItemTemplate*>& Templates, 
    FString& OutErrorMessage)
{
    if (Templates.Num() == 0)
    {
        OutErrorMessage = TEXT("No templates to export");
        return false;
    }

    const bool bMultipleTemplates = Templates.Num() > 1;
    const FString fileExtension = bMultipleTemplates ? TEXT(".mnteaitems") : TEXT(".mnteaitem");
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
    TArray<FString> validJsonData;
    for (const auto* Template : Templates)
    {
        if (!IsValid(Template))
            continue;

        const FString jsonData = Template->GetJson();
        if (!jsonData.IsEmpty())
            validJsonData.Add(jsonData);
    }

    if (validJsonData.Num() == 0)
    {
        OutErrorMessage = TEXT("Selected templates have no JSON data to export");
        return false;
    }

    const bool bMultipleTemplates = validJsonData.Num() > 1;
    const FString fileExtension = bMultipleTemplates ? TEXT(".mnteaitems") : TEXT(".mnteaitem");

    FString finalPath = FilePath;
    if (!finalPath.EndsWith(fileExtension))
        finalPath += fileExtension;

    FString finalJson;
    if (bMultipleTemplates)
    {
        finalJson = TEXT("{\n\t\"items\": [\n");
        for (int32 i = 0; i < validJsonData.Num(); ++i)
        {
            finalJson += TEXT("\t\t") + validJsonData[i];
            if (i < validJsonData.Num() - 1)
                finalJson += TEXT(",");
            finalJson += TEXT("\n");
        }
        finalJson += TEXT("\t]\n}");
    }
    else
        finalJson = validJsonData[0];

    if (!FFileHelper::SaveStringToFile(finalJson, *finalPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
    {
        OutErrorMessage = TEXT("Failed to write export file");
        return false;
    }

    return true;
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
    FString& OutErrorMessage)
{
    if (!IsValid(Template))
    {
        OutErrorMessage = TEXT("Invalid template object for update");
        return false;
    }

    if (!ParseSingleTemplateJson(JsonString, Template, OutErrorMessage))
        return false;

    Template->CalculateJson();
    
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
    if (jsonObject->TryGetStringField(TEXT("Guid"), guidString))
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
    {
        displayName.RemoveSpacesInline();

        const FRegexPattern InvalidCharsPattern(TEXT("[\\\\:\\*\\?\"<>\\| ,.&!~@#']"));
        FRegexMatcher Matcher(InvalidCharsPattern, displayName);

        FString Sanitized;
        Sanitized.Reserve(displayName.Len());

        int32 LastIndex = 0;
        while (Matcher.FindNext())
        {
            const int32 MatchBegin = Matcher.GetMatchBeginning();
            Sanitized += displayName.Mid(LastIndex, MatchBegin - LastIndex);
            LastIndex = Matcher.GetMatchEnding();
        }
        Sanitized += displayName.Mid(LastIndex);

        return Sanitized;
    }

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

    if (!DeserializeTemplateFromJson(jsonObject, Template, OutErrorMessage))
        return false;

    Template->SetJson(JsonString);
    
    return true;
}

bool UMounteaAdvancedInventoryItemTemplateEditorStatics::ParseMultipleTemplatesJson(
    const FString& JsonString, 
    TArray<FString>& OutItemJsons, 
    FString& OutErrorMessage)
{
    TSharedPtr<FJsonObject> rootObject;
    TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(jsonReader, rootObject) || !rootObject.IsValid())
    {
        OutErrorMessage = TEXT("Failed to parse JSON");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* itemsArray;
    if (!rootObject->TryGetArrayField(TEXT("items"), itemsArray))
    {
        OutErrorMessage = TEXT("JSON missing 'items' array");
        return false;
    }

    for (const auto& ItemValue : *itemsArray)
    {
        if (ItemValue->Type == EJson::Object)
        {
            FString itemJsonString;
            TSharedRef<TJsonWriter<>> jsonWriter = TJsonWriterFactory<>::Create(&itemJsonString);
            FJsonSerializer::Serialize(ItemValue->AsObject().ToSharedRef(), jsonWriter);
            OutItemJsons.Add(itemJsonString);
        }
    }

    return OutItemJsons.Num() > 0;
}

UMounteaInventoryItemTemplate* UMounteaAdvancedInventoryItemTemplateEditorStatics::CreateTemplateAsset(
    const FString& TargetFolder, 
    const FString& AssetName, 
    FString& OutErrorMessage)
{
    const FString packageName = FPaths::Combine(TargetFolder, AssetName);
    UPackage* newPackage = CreatePackage(*packageName);
    
    if (!newPackage)
    {
        OutErrorMessage = FString::Printf(TEXT("Failed to create package: %s"), *packageName);
        return nullptr;
    }

    UMounteaInventoryItemTemplate* newTemplate = NewObject<UMounteaInventoryItemTemplate>(
        newPackage, 
        UMounteaInventoryItemTemplate::StaticClass(),
        *AssetName,
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
    if (!IsValid(Template))
        return false;

    UPackage* templatePackage = Template->GetOutermost();
    templatePackage->FullyLoad();
    templatePackage->MarkPackageDirty();

    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    saveArgs.SaveFlags = SAVE_NoError;

    const FString packageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
    
    if (!UPackage::SavePackage(templatePackage, Template, *packageFileName, saveArgs))
        return false;

    FAssetRegistryModule::AssetCreated(Template);
    
    return true;
}

FString UMounteaAdvancedInventoryItemTemplateEditorStatics::ShowOpenFileDialog(
    const FString& DialogTitle, 
    const FString& FileTypes)
{
    IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
    if (!desktopPlatform)
    {
        return FString();
    }

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
    
    TSharedRef<SWindow> pickerWindow = SNew(SWindow)
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
                        pickerWindow->RequestDestroyWindow();
                        return FReply::Handled();
                    })
                ]
            ]
        ];

    GEditor->EditorAddModalWindow(pickerWindow);
    
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
