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
#include "MounteaAdvancedInventorySystemEditorStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API
	UMounteaAdvancedInventorySystemEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void OpenSettings(const FString& ContainerName, const FString& CategoryName, const FString& SectionName);
	static void OpenAsset(const FString& AssetPath);
	
	static void OpenInventoryConfig();
	static void OpenEquipmentConfig();
	
	static void OpenInventoryUIConfig();
};
