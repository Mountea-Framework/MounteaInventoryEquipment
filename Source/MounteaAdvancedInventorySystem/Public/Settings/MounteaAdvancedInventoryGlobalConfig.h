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
#include "EdGraph/EdGraphPin.h"
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryGlobalConfig.generated.h"

UENUM(BlueprintType)
enum class EMounteaJsonDefinitionFieldType : uint8
{
	Int			UMETA(DisplayName="Int"),
	Int64		UMETA(DisplayName="Int64"),
	Float		UMETA(DisplayName="Float"),
	Bool		UMETA(DisplayName="Bool"),
	String		UMETA(DisplayName="String"),
	Name		UMETA(DisplayName="Name"),
	Text		UMETA(DisplayName="Text"),
	Byte		UMETA(DisplayName="Byte"),
	Object		UMETA(DisplayName="Object"),
	SoftObject	UMETA(DisplayName="Soft Object"),
	Class		UMETA(DisplayName="Class"),
	SoftClass	UMETA(DisplayName="Soft Class"),
	JsonObject	UMETA(DisplayName="Json Object"),
	Struct		UMETA(DisplayName="Struct")
};

USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaJsonObjectDefinitionField
{
	GENERATED_BODY()

public:

	FMounteaJsonObjectDefinitionField()
	{
		FieldValueType.PinCategory = TEXT("string");
		FieldValueType.ContainerType = EPinContainerType::None;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault))
	FName FieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault, HideInDetailPanel))
	EMounteaJsonDefinitionFieldType FieldType = EMounteaJsonDefinitionFieldType::String;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault))
	FEdGraphPinType FieldValueType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault))
	bool bRequired = true;

public:

	bool IsValid() const
	{
		return !FieldName.IsNone();
	}
};

USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaJsonObjectDefinitionInclude
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault))
	FString DefinitionKey;

public:

	bool IsValid() const
	{
		return !DefinitionKey.IsEmpty();
	}
};

USTRUCT(BlueprintType)
struct MOUNTEAADVANCEDINVENTORYSYSTEM_API FMounteaJsonObjectDefinition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault),
		meta=(TitleProperty="{DefinitionKey}"))
	TArray<FMounteaJsonObjectDefinitionInclude> IncludedDefinitions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JSON Definition",
		meta=(NoResetToDefault),
		meta=(TitleProperty="{FieldName} | {FieldType}"))
	TArray<FMounteaJsonObjectDefinitionField> Fields;

public:

	bool IsValid() const
	{
		return Fields.Num() > 0;
	}
};

UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Global Config",
	meta=(ShortTooltip="Global configuration asset for Mountea Inventory & Equipment shared definitions."))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryGlobalConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMounteaAdvancedInventoryGlobalConfig();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="JSON Definitions",
		meta=(ForceInlineRow, NoResetToDefault))
	TMap<FString, FMounteaJsonObjectDefinition> JsonObjectDefinitions;
};
