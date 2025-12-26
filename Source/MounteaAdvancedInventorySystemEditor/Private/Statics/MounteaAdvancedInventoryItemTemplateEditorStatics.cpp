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

    FString fileContent;
    if (!FFileHelper::LoadFileToString(fileContent, *filePath))
    {
        OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *filePath);
        return false;
    }

    const bool bIsMultipleFile = filePath.EndsWith(TEXT(".mnteaitems"));
    
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

        if (UMounteaInventoryItemTemplate* targetTemplate = FindTemplateByGuid(existingTemplates, itemGuid))
        {
            if (UpdateExistingTemplate(targetTemplate, itemJson, OutErrorMessage))
            {
                OutTemplates.Add(targetTemplate);
                updatedCount++;
            }
        }
        else
        {
            const FString targetFolder = ShowContentBrowserPathPicker(
                NSLOCTEXT("UMounteaAdvancedInventoryItemTemplateEditorStatics", "Import_TargetFolder", 
                    "Select Target Folder for New Template").ToString(),
                TEXT("/Game/")
            );

            if (targetFolder.IsEmpty())
            {
                OutErrorMessage = TEXT("No target folder selected for new template");
                continue;
            }

            const FString assetName = FString::Printf(TEXT("ImportedTemplate_%s"), *itemGuid.ToString(EGuidFormats::Short));
            UMounteaInventoryItemTemplate* newTemplate = CreateTemplateAsset(targetFolder, assetName, OutErrorMessage);
            
            if (!newTemplate)
                continue;

            if (ParseSingleTemplateJson(itemJson, newTemplate, OutErrorMessage))
            {
                newTemplate->CalculateJson();
                
                const FString packagePath = FPaths::Combine(targetFolder, assetName);
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

    TArray<FString> validJsonData;
    for (const auto* Template : Templates)
    {
        if (!IsValid(Template))
            continue;

        const FString JsonData = Template->GetJson();
        if (!JsonData.IsEmpty())
            validJsonData.Add(JsonData);
    }

    if (validJsonData.Num() == 0)
    {
        OutErrorMessage = TEXT("Selected templates have no JSON data to export");
        return false;
    }

    const bool bMultipleTemplates = validJsonData.Num() > 1;
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

    FString finalPath = filePath;
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
