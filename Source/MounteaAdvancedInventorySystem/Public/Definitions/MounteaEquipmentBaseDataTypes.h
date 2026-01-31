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
#include "Hash/Blake3.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentBaseDataTypes.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct FMounteaEquipmentSlotHeaderData
{
	GENERATED_BODY()
	
public:
	
	FMounteaEquipmentSlotHeaderData() : 
		bIsEnabled(1)
	{};
	
	void RegenerateSlotId()
	{
		// Get stable, sorted lists of tags
		TArray<FGameplayTag> slotTags;
		TagContainer.GetGameplayTagArray(slotTags);
		slotTags.Sort([](const FGameplayTag& A, const FGameplayTag& B)
		{
			return A.GetTagName().LexicalLess(B.GetTagName());
		});

		TArray<FGameplayTag> blockedTags;
		BlackedByTags.GetGameplayTagArray(blockedTags);
		blockedTags.Sort([](const FGameplayTag& A, const FGameplayTag& B)
		{
			return A.GetTagName().LexicalLess(B.GetTagName());
		});

		// Build a stable string key
		TStringBuilder<512> stringBuilder;
		stringBuilder.Append(TEXT("enabled="));
		stringBuilder.Append(bIsEnabled ? TEXT("1") : TEXT("0"));

		stringBuilder.Append(TEXT("|tags="));
		for (const FGameplayTag& T : slotTags)
		{
			stringBuilder.Append(T.GetTagName().ToString());
			stringBuilder.AppendChar(TEXT(';'));
		}

		stringBuilder.Append(TEXT("|blocked="));
		for (const FGameplayTag& T : blockedTags)
		{
			stringBuilder.Append(T.GetTagName().ToString());
			stringBuilder.AppendChar(TEXT(';'));
		}

		// Hash UTF-8 bytes
		const FString stringKey = stringBuilder.ToString();
		const FTCHARToUTF8 utf8Key(*stringKey);

		const FBlake3Hash finaleHash = FBlake3::HashBuffer(utf8Key.Get(), static_cast<int64>(utf8Key.Length()));
		SlotId = FGuid::NewGuidFromHash(finaleHash);
	}
	
	void PostSerialize(const FArchive& Ar)
	{
		if (!SlotId.IsValid())
			RegenerateSlotId();
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(Categories="Mountea_Inventory"))
	FGameplayTagContainer TagContainer;
	
	// Eg.: `LeftHand` is blocked by `TwoHanded` weapons etc.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(Categories="Mountea_Inventory"))
	FGameplayTagContainer BlackedByTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsEnabled : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid SlotId;
};

FORCEINLINE bool operator==(const FMounteaEquipmentSlotHeaderData& A, const FMounteaEquipmentSlotHeaderData& B)
{
	return A.SlotId == B.SlotId;
}

FORCEINLINE bool operator!=(const FMounteaEquipmentSlotHeaderData& A, const FMounteaEquipmentSlotHeaderData& B)
{
	return !(A == B);
}

FORCEINLINE uint32 GetTypeHash(const FMounteaEquipmentSlotHeaderData& Data)
{
	return GetTypeHash(Data.SlotId);
}

template<>
struct TStructOpsTypeTraits<FMounteaEquipmentSlotHeaderData>
	: public TStructOpsTypeTraitsBase2<FMounteaEquipmentSlotHeaderData>
{
	enum
	{
		WithPostSerialize = true
	};
};