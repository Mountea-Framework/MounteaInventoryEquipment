// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MounteaEquipmentHelpers.generated.h"

#define LOCTEXT_NAMESPACE "MounteaEquipmentSlots"

class UMounteaInventoryItemBase;

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlots
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta=(DisplayThumbnail=false))
	FString SlotID;

	UPROPERTY(VisibleAnywhere, meta=(DisplayThumbnail=false))
	UMounteaInventoryItemBase* Item;

public:

	bool operator==(const FMounteaEquipmentSlots& Other) const
	{
		return SlotID == Other.SlotID;
	}

	bool operator!=(const FMounteaEquipmentSlots& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FMounteaEquipmentSlots& Data)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Data.SlotID));
				
		return Hash;
	}
};
