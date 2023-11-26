// Copyright Dominik Morse 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentHelpers.generated.h"

struct FMounteaEquipmentSlotCompare;
class UMounteaInstancedItem;

#define LOCTEXT_NAMESPACE "MounteaEquipmentSlots"

#pragma region EquipmentSlot

USTRUCT(BlueprintType, Blueprintable, DisplayName="Equipment Slot")
struct FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot() : Item(nullptr), SlotGuid(FGuid()), SlotName(FText())
	{};
	
	FEquipmentSlot(const FEquipmentSlot& Other) :
		Item(Other.Item), SlotGuid(Other.SlotGuid), SlotName(Other.SlotName), SlotTags(Other.SlotTags)
	{};

	FEquipmentSlot(UMounteaInstancedItem* NewItem);

public:

	/**
	 * A reference to the actual unique item that this slot contains. It points to an instance of an item, defining what item
	 * the slot is holding and providing access to its data and behaviors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(DisplayThumbnail=false))
	UMounteaInstancedItem* Item = nullptr;

	/**
	 * The identifier for the slot. 
	 * Slot Guid represents the Item's Guid which resides in the Slot. Item Guid and Slot Guid are always the same.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid SlotGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer SlotTags;

public:

	void UpdateSlot(UMounteaInstancedItem* NewItem);
	
	/** Check if the slot is empty, meaning it has no Item. */
	bool IsEmpty() const
	{ return Item == nullptr; };

	/** Checks if the slot is valid, meaning it has an item and at least one stack. */
	bool IsValid() const
	{
		const bool bIsEmpty = IsEmpty();
		const bool bHasGuid = SlotGuid.IsValid();
				
		return bIsEmpty && bHasGuid;
	}

	bool operator==(const FEquipmentSlot& Other) const;

	bool operator==(const FEquipmentSlot* Other) const;

	bool operator==(const FGuid& Other) const;
	
	bool operator==(const FText& Other) const;

	bool operator==(const UMounteaInstancedItem* Other) const;

	bool operator==(const FMounteaEquipmentSlotCompare& Other) const;
};

template<>
struct TStructOpsTypeTraits< FEquipmentSlot > : public TStructOpsTypeTraitsBase2< FEquipmentSlot >
{
	enum
	{
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

FORCEINLINE  uint32 GetTypeHash(const FEquipmentSlot& Data)
{
	return GetTypeHash(Data.SlotGuid);
};

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlotCompare
{
	GENERATED_BODY()

	FMounteaEquipmentSlotCompare() : SlotItem(nullptr), SlotID(FText())
	{};
	
	FMounteaEquipmentSlotCompare(const UMounteaInstancedItem* Item, const FText& ID) : SlotItem(Item), SlotID(ID)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UMounteaInstancedItem* SlotItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotID;
};

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlotIdentity
{
	GENERATED_BODY()

	FMounteaEquipmentSlotIdentity() : SlotName(FText()), SlotTags(FGameplayTagContainer())
	{};

	FMounteaEquipmentSlotIdentity(const FText& Name, const FGameplayTagContainer& Tags) : SlotName(Name), SlotTags(Tags)
	{};

	FMounteaEquipmentSlotIdentity(const FMounteaEquipmentSlotIdentity& Other) : SlotName(Other.SlotName), SlotTags(Other.SlotTags)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer SlotTags;

	bool operator==(const FText& Other) const;
};

#pragma endregion 

#undef LOCTEXT_NAMESPACE