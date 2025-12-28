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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaAdvancedInventoryItemTemplateEditorStatics.generated.h"

struct FGameplayTagContainer;
class UMounteaInventoryItemTemplate;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API
	UMounteaAdvancedInventoryItemTemplateEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool ImportTemplatesFromFile(TArray<UMounteaInventoryItemTemplate*>& OutTemplates, FString& OutErrorMessage);
	static bool ExportTemplatesToFile(const TArray<UMounteaInventoryItemTemplate*>& Templates, FString& OutErrorMessage);

public:
	static TArray<UMounteaInventoryItemTemplate*> LoadAllExistingTemplates();
	static UMounteaInventoryItemTemplate* FindTemplateByGuid(const TArray<UMounteaInventoryItemTemplate*>& Templates, const FGuid& Guid);
	static bool UpdateExistingTemplate(UMounteaInventoryItemTemplate* Template, const FString& JsonString, FString& OutErrorMessage);
	static FGuid ExtractGuidFromJson(const FString& JsonString);
	
	static bool ParseSingleTemplateJson(const FString& JsonString, UMounteaInventoryItemTemplate* Template, FString& OutErrorMessage);
	static bool ParseMultipleTemplatesJson(const FString& JsonString, TArray<FString>& OutItemJsons, FString& OutErrorMessage);
    
	static UMounteaInventoryItemTemplate* CreateTemplateAsset(const FString& TargetFolder, const FString& AssetName, FString& OutErrorMessage);
	static bool SaveTemplateAsset(UMounteaInventoryItemTemplate* Template, const FString& PackagePath);
    
	static FString ShowOpenFileDialog(const FString& DialogTitle, const FString& FileTypes);
	static FString ShowSaveFileDialog(const FString& DialogTitle, const FString& DefaultFileName, const FString& FileTypes);
	static FString ShowFolderDialog(const FString& DialogTitle, const FString& DefaultPath);
	static FString ShowContentBrowserPathPicker(const FString& DialogTitle, const FString& DefaultPath);

	static bool DeserializeTemplateFromJson(const TSharedPtr<FJsonObject>& JsonObject, UMounteaInventoryItemTemplate* Template, FString& OutErrorMessage);
	static bool DeserializeGameplayTagContainer(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, FGameplayTagContainer& OutContainer);
	static bool DeserializeSoftClassPtrSet(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, TSet<TSoftClassPtr<UObject>>& OutSet);
};
