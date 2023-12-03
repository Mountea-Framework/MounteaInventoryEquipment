// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaEquipmentConfigData.h"


#include "GameplayTagAssetInterface.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"

#if WITH_EDITOR
#include "GameplayTagsManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#endif

#define LOCTEXT_NAMESPACE "MounteaEquipmentConfigData"

#if WITH_EDITOR

void UMounteaEquipmentConfigData::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	// TODO: If EquipmentSlotIDs are about to change, we must cache out all places where current value is used and update it to the new value
}

void UMounteaEquipmentConfigData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.GetPropertyName() : NAME_None;
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaEquipmentConfigData, EquipmentSlotIDs) || MemberPropertyName == GET_MEMBER_NAME_CHECKED(UMounteaEquipmentConfigData, EquipmentSlotIDs) )
	{
		TArray<FText> ErrorTexts;
		TSet<FMounteaEquipmentSlotIdentity> DirtySlots;
		
		ValidateConfig(ErrorTexts, DirtySlots);

		TSet<FMounteaEquipmentSlotIdentity> NewSlots = EquipmentSlotIDs;

		if (DirtySlots.Num() > 0)
		{
			for (auto& Itr : DirtySlots)
			{
				if (FMounteaEquipmentSlotIdentity* Slot = EquipmentSlotIDs.Find(Itr))
				{	
					Slot->Reset();
					
					EquipmentSlotIDs.Remove(*Slot);
					
					OnTagModified.ExecuteIfBound(Itr.SlotTag);
				}				
			}
			
			EquipmentSlotIDs.Add(FMounteaEquipmentSlotIdentity());

			if (PropertyName == GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotTag))
			{
				OnPropertyModified.ExecuteIfBound(PropertyChangedEvent.Property);
			}
		}
	}

	// TODO: If EquipmentSlotIDs are about to change, we must cache out all places where current value is used and update it to the new value
}

EDataValidationResult UMounteaEquipmentConfigData::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);
	TSet<FMounteaEquipmentSlotIdentity> DirtySlots;

	Result = ValidateConfig(ValidationErrors, DirtySlots) ? Result : EDataValidationResult::Invalid;
	
	return Result;
}

#endif

bool UMounteaEquipmentConfigData::ValidateConfig(TArray<FText>& Notifications, TSet<FMounteaEquipmentSlotIdentity>& DirtySlots)
{
	for (const auto& Itr : EquipmentSlotIDs)
	{
		int32 HowMany = 0;

		for (const auto& Itr2 : EquipmentSlotIDs)
		{
			if (Itr == Itr2)
			{
				HowMany++;

				if (HowMany > 1)
				{						
					DirtySlots.Add(Itr);
				}					
			}
		}

		if (Itr.SlotTag.IsValid() == false && Itr.SlotName.IsEmpty())
		{
			DirtySlots.Add(Itr);
		}
	}

	const bool bSatisfied = DirtySlots.Num() > 0;
	
#if WITH_EDITOR
	for (const auto& Itr : DirtySlots)
	{
		if (const FMounteaEquipmentSlotIdentity* Slot = DirtySlots.Find(Itr))
		{			
			FText Message = LOCTEXT("MounteaEquipmentConfigData_DuplicateElement_Title", "Duplicate elements are not allowed in Equipment Slots.");
			if (Slot->IsEmpty())
			{
				Message = LOCTEXT("MounteaEquipmentConfigData_DuplicateElement_Title_Empty", "Empty elements are not allowed in Equipment Slots.");
			}
				
			FNotificationInfo InvalidOperation(Message);
			{
				InvalidOperation.SubText = LOCTEXT("MounteaEquipmentConfigData_DuplicateElement_Body", "Please ensure each Slot has unique Tag and Name!");
				InvalidOperation.bUseThrobber = true;
				InvalidOperation.bUseSuccessFailIcons = true;
				InvalidOperation.ExpireDuration = 3.0f;
				InvalidOperation.bFireAndForget = true;
				InvalidOperation.bAllowThrottleWhenFrameRateIsLow = true;
				InvalidOperation.Hyperlink = FSimpleDelegate::CreateLambda([]()
				{
					FPlatformProcess::LaunchURL(*MounteaInventoryEquipmentConsts::Hyperlinks::EquipmentConfigURL, nullptr, nullptr);
				});
				InvalidOperation.HyperlinkText = LOCTEXT("MounteaEquipmentConfigData_DuplicityLink", "Open Documentation");
					
			}
			FSlateNotificationManager::Get().AddNotification(InvalidOperation);

#endif
		}			
	}

	return bSatisfied;
}


#undef LOCTEXT_NAMESPACE
