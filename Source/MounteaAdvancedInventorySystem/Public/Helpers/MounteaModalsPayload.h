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
#include "Definitions/MounteaAdvancedInventoryGlobalUIDataTypes.h"
#include "UObject/Object.h"
#include "MounteaModalsPayload.generated.h"


UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName="Mountea Modals Payload"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaModalsPayload : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	FString ModalType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	FMounteaModalsConfig ModalConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Secondary Data")
	TObjectPtr<UObject> OptionalPayload;
};

UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName="Mountea Modals Payload (Slider)"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaModalSlidersPayload : public UMounteaModalsPayload
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 MaxSliderQuantity = 99;
};

//
// ANSWERS
//
	
UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName="Mountea Modal Response Payload"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaModalResponsePayload : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * True: Confirmed
	 * False: Rejected
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	uint8 bConfirmationType : 1;

};

UCLASS(ClassGroup=(Mountea), Blueprintable, BlueprintType, meta=(DisplayName="Mountea Slider Modal Response Payload"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaModalSliderResponsePayload : public UMounteaModalResponsePayload
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary Data")
	int32 Quantity = 0;
};
