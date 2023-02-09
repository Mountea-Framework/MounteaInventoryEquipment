// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/ActorInventoryItemComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryPluginLog.h"

#if WITH_EDITOR
#include "EditorHelper.h"
#endif

void UActorInventoryItemComponent::BeginPlay()
{
	Super::BeginPlay();

	{
		if (SetupMode == EInventoryItemSetup::EIIS_FromItem)
		{
			SourceItem = SourceItemClass.GetDefaultObject();
			if (( SourceItem && !SourceItem->IsValidItem() ) || !GetOwner() || !SourceItem)
			{
				SourceItem = nullptr;
				if (GetOwner())
				{
					AInvP_LOG(Error, TEXT("[UActorInventoryItemComponent] %s owned by %s contains Invalid Data definition!"), *GetName(), *GetOwner()->GetName())
				}

				Deactivate();
			}
		}

		if (SetupMode == EInventoryItemSetup::EIIS_FromDataTable)
		{
			if (const auto DataTable = SourceItemRow.DataTable)
			{
				if (DataTable->GetRowStruct())
				{
					const UScriptStruct* InventoryRowStruct = DataTable->GetRowStruct();
					if (!(InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct())))
					{
						if (GetOwner())
						{
							AInvP_LOG(Error, TEXT("[UActorInventoryItemComponent] %s owned by %s contains Invalid Data Structure!"), *GetName(), *GetOwner()->GetName())
						}

						Deactivate();
					}
					else
					{
						SourceItem = NewObject<UInventoryItem>();
						SourceItem->SetItem(GetItemDefinition());
					}
				}
			}
		}
	}
	
	OnUpdateSpawnedActorItem.AddUniqueDynamic(this, &UActorInventoryItemComponent::UpdateSpawnedActorItem);
}

FInventoryItemData UActorInventoryItemComponent::GetItemDefinition() const
{
	FInventoryItemData ReturnDefinition;
	switch (SetupMode)
	{
		case EInventoryItemSetup::EIIS_FromItem:
			if (SourceItem)
			{
				return SourceItem->GetItem();
			}
			break;
		case EInventoryItemSetup::EIIS_FromDataTable:
			if (SourceItemRow.DataTable)
			{
				static const FString ContextString(TEXT(""));
				const FInventoryItemData* Row = SourceItemRow.DataTable->FindRow<FInventoryItemData>(SourceItemRow.RowName, ContextString);
				if (ContextString.IsEmpty())
				{
					ReturnDefinition = *Row;
				}
			}
			break;
		case EInventoryItemSetup::EIIS_Default:
			break;
		default: ;
	}

	return ReturnDefinition;
}

FOnUpdateSpawnedActorItem& UActorInventoryItemComponent::GetSpawnActorRequestedHandle()
{
	return OnUpdateSpawnedActorItem;
}

void UActorInventoryItemComponent::UpdateSpawnedActorItem(const FInventoryItemData InventoryItemData)
{
	if (SourceItem)
	{
		SourceItem->SetItem(InventoryItemData);
	}
}

#if WITH_EDITOR

void UActorInventoryItemComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == "DataTable")
	{
		if (SetupMode == EInventoryItemSetup::EIIS_FromDataTable)
		{
			if (const auto DataTable = SourceItemRow.DataTable)
			{
				if (DataTable->GetRowStruct())
				{
					const UScriptStruct* InventoryRowStruct = DataTable->GetRowStruct();
					if (!(InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct())))
					{
						const FString ErrorMessage = FString::Printf(TEXT("INVALID DATA TABLE: %s structure is not supported!"), *InventoryRowStruct->GetDisplayNameText().ToString());
						FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
					}
				}
			}
		}
	}

	if (PropertyName == "SourceItemClass")
	{
		if (!SourceItemClass)
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM Class: Source Item Class must be selected!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
		if (SourceItemClass && !(SourceItemClass->IsChildOf(UInventoryItem::StaticClass() ) ) )
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM Class: Valid Item Class must be selected!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
		/** Commented out as using Class Template is better for scaling.
		if (!SourceItem)
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM: Source Item must be selected!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}

		if (SourceItem && !SourceItem->IsValidItem())
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM: Source Item is not valid!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
		*/
	}
}

EDataValidationResult UActorInventoryItemComponent::IsDataValid(TArray<FText>& ValidationErrors)
{
	if (Super::IsDataValid(ValidationErrors) == EDataValidationResult::Invalid)
	{
		return EDataValidationResult::Invalid;	
	}
	
	if (SetupMode == EInventoryItemSetup::EIIS_FromItem)
	{
		if (!SourceItemClass)
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM Class: Source Item Class must be selected!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));
		}
		if (SourceItemClass && !(SourceItemClass->IsChildOf(UInventoryItem::StaticClass() ) ) )
		{
			const FString ErrorMessage = FString::Printf(TEXT("INVALID SOURCE ITEM Class: Valid Item Class must be selected!"));
			FEditorHelper::DisplayEditorNotification(FText::FromString(ErrorMessage), SNotificationItem::CS_Fail, 5.f, 2.f, TEXT("Icons.Error"));

		}
		
		/** Commented out as using Class Template is better for scaling.
		if (!SourceItem || ( SourceItem && !SourceItem->IsValidItem() ))
		{
			ValidationErrors.Add(FText::FromString(TEXT("Source Item is not Valid!")));
			return EDataValidationResult::Invalid;
		}
		*/
	}

	if (SetupMode == EInventoryItemSetup::EIIS_FromDataTable)
	{
		if (const auto DataTable = SourceItemRow.DataTable)
		{
			if (DataTable->GetRowStruct())
			{
				const UScriptStruct* InventoryRowStruct = DataTable->GetRowStruct();
				if (!(InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct())))
				{
					ValidationErrors.Add(FText::FromString(TEXT("Source Structure is not Supported!")));
					return EDataValidationResult::Invalid;
				}
			}
		}
	}
	
	return EDataValidationResult::Valid;
}

#endif
