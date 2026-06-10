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
#include "MounteaAdvancedInventoryGlobalUIDataTypes.generated.h"

class UMounteaModalResponsePayload;
class UMounteaModalsPayload;
class UUserWidget;

/**
 * Defines the content widget and payload class used by one modal type.
 */
USTRUCT(BlueprintType)
struct FMounteaModalDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault),
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaAdvancedInventoryModalContentWidgetInterface"),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UMounteaModalsPayload> PayloadClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault),
		meta=(ForceShowPluginContent),
		meta=(ForceShowEngineContent))
	TSoftClassPtr<UMounteaModalResponsePayload> PayloadResponseClass;
};

/**
 * A structure that represents the configuration for modal dialogs.
 *
 * This structure is derived from FTableRowBase and is intended to be used in
 * conjunction with data tables for managing modal dialog configurations.
 *
 * The configuration includes properties for specifying the title and body
 * content of the modal dialog.
 */
USTRUCT(BlueprintType)
struct FMounteaModalsConfig : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault))
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault),
		meta=(Multiline=true))
	FText Body;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault))
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault))
	uint8 bAutoClose : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Modals",
		meta=(NoResetToDefault),
		meta=(EditCondition="bAutoClose", EditConditionHides),
		meta=(Units="s"),
		meta=(UIMin=0,ClampMin=0))
	int32 ModalDuration = INDEX_NONE;
};
