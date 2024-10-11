// All rights reserved Dominik Morse (Pavlicek) 2024

#include "Definitions/MounteaInventoryItem.h"

UMounteaInventoryItem::UMounteaInventoryItem()
{
	FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UMounteaInventoryItem::PostWorldCreated);
}

UMounteaInventoryItem::~UMounteaInventoryItem()
{
	FWorldDelegates::OnPostWorldCreation.RemoveAll(this);
}

void UMounteaInventoryItem::PostWorldCreated(UWorld* NewWorld)
{
	if (FApp::IsGame())
	{
		SetWorld(NewWorld);

		OnItemAdded.AddUniqueDynamic		(this, &UMounteaInventoryItem::ItemAdded);
		OnItemInitialized.AddUniqueDynamic	(this, &UMounteaInventoryItem::ItemInitialized);
		OnItemModified.AddUniqueDynamic		(this, &UMounteaInventoryItem::ItemModified);
		OnItemRemoved.AddUniqueDynamic		(this, &UMounteaInventoryItem::ItemRemoved);

		OnItemBeginPlay(TEXT("Item has been initialized"));
	}
}

void UMounteaInventoryItem::SetWorldFromLevel(ULevel* FromLevel)
{
	if (FromLevel == nullptr)
	{
		return;
	}
	
	if (FromLevel->GetWorld() != World)
	{
		SetWorld(FromLevel->GetWorld());
	}
}

void UMounteaInventoryItem::SetWorld(UWorld* NewWorld)
{
	World = NewWorld;
}

void UMounteaInventoryItem::OnItemBeginPlay_Implementation(const FString& Message)
{
	OnItemInitialized.Broadcast(Message);
}

void UMounteaInventoryItem::ItemAdded(const FString& Message)
{
	
}

void UMounteaInventoryItem::ItemInitialized(const FString& Message)
{
	
}

void UMounteaInventoryItem::ItemModified(const FString& Message)
{
	
}

void UMounteaInventoryItem::ItemRemoved(const FString& Message)
{
	
}

#if WITH_EDITOR

void UMounteaInventoryItem::PostDuplicate(bool bDuplicateForPIE)
{
	UObject::PostDuplicate(bDuplicateForPIE);
}

EDataValidationResult UMounteaInventoryItem::IsDataValid(FDataValidationContext& Context)
{
	return UObject::IsDataValid(Context);
}

void UMounteaInventoryItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif