// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaEquipmentConfigData.h"


#include "GameplayTagAssetInterface.h"
#include "Helpers/MounteaInventoryEquipmentConsts.h"

#if WITH_EDITOR
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
		TArray<FValidationResult> ValidationResults;
		
		ValidateConfig(ValidationResults);

		TSet<FMounteaEquipmentSlotIdentity> NewSlots = EquipmentSlotIDs;

		if (ValidationResults.Num() > 0)
		{
			for (auto& [ValidationText, BrokenIdentity, Result, Index] : ValidationResults)
			{
				if (Result != EIVR_Error) continue;
				
				if (FMounteaEquipmentSlotIdentity* Slot = EquipmentSlotIDs.Find(BrokenIdentity))
				{	
					Slot->Reset();
					
					EquipmentSlotIDs.Remove(*Slot);
					
					OnTagModified.ExecuteIfBound(BrokenIdentity.SlotTag);

					EquipmentSlotIDs.Add(FMounteaEquipmentSlotIdentity());
				}				
			}
		}
	}

	// TODO: If EquipmentSlotIDs are about to change, we must cache out all places where current value is used and update it to the new value
}

EDataValidationResult UMounteaEquipmentConfigData::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);

	TArray<FValidationResult> ValidationResults;
	
	Result = ValidateConfig(ValidationResults) ? Result : EDataValidationResult::Invalid;
	
	return Result;
}

#endif

bool UMounteaEquipmentConfigData::ValidateConfig(TArray<FValidationResult>& Results)
{
	bool bSatisfied = true;

	TArray<FMounteaEquipmentSlotIdentity> Identities = EquipmentSlotIDs.Array();
	
	for (const auto& Itr : EquipmentSlotIDs)
	{
		int32 HowMany = 0;

		const auto Index = Identities.Find(Itr);
		
		FValidationResult Result;
		Result.Index = Index;

		for (const auto& Itr2 : EquipmentSlotIDs)
		{
			if (Itr == Itr2)
			{
				HowMany++;

				if (HowMany > 1)
				{
					Result.ValidationText = LOCTEXT("IdentityValidationResult_Error_Duplicate", "Duplicate elements are not allowed in Equipment Slots.");
					Result.BrokenIdentity = Itr;
					Result.Result = EIVR_Error;
					
					Results.Add(Result);

					bSatisfied = false;
				}					
			}
		}
		
		if (Itr.SlotTag.IsValid() == false && Itr.SlotName.IsEmpty())
		{
			Result.ValidationText = LOCTEXT("IdentityValidationResult_Error_NoNameAndTag", "Empty elements are not allowed in Equipment Slots.");
			Result.BrokenIdentity = Itr;
			Result.Result = EIVR_Error;
			Results.Add(Result);

			bSatisfied = false;
		}
		else if (Itr.SlotTag.IsValid() == false && Itr.SlotName.IsEmpty() == false)
		{
			Result.ValidationText = LOCTEXT("IdentityValidationResult_Warning_NoTag", "Empty `Slot Tag` elements are not allowed in Equipment Slots.");
			Result.BrokenIdentity = Itr;
			Result.Result = EIVR_Warning;
			Results.Add(Result);
		}
		else if (Itr.SlotTag.IsValid() && Itr.SlotName.IsEmpty())
		{
			Result.ValidationText = LOCTEXT("IdentityValidationResult_Warning_NoName", "Empty `Slot Name` elements are not allowed in Equipment Slots.");
			Result.BrokenIdentity = Itr;
			Result.Result = EIVR_Warning;
			Results.Add(Result);
		}

		if (Itr.SlotCompatibleTags.IsEmpty())
		{
			Result.ValidationText = LOCTEXT("IdentityValidationResult_Warning_NoTags", "Empty `Slot Compatible Tags` elements are not allowed in Equipment Slots.");
			Result.BrokenIdentity = Itr;
			Result.Result = EIVR_Warning;
			Results.Add(Result);
		}
	}

#if WITH_EDITOR
	
	for (const auto& [ValidationText, BrokenIdentity, Result, Index] : Results)
	{
		FText Message = ValidationText;
				
		FNotificationInfo InvalidOperation(Message);
		{
			InvalidOperation.SubText = FText::Format(LOCTEXT("MounteaEquipmentConfigData_DuplicateElement_Body", "Please ensure each Slot has unique Tag and Name.\nInvalid index: {0}"), Index);
			InvalidOperation.bUseThrobber = true;
			InvalidOperation.bUseSuccessFailIcons = Result == EIVR_Error ? false : true;
			InvalidOperation.ExpireDuration = 3.0f;
			InvalidOperation.bFireAndForget = true;
			InvalidOperation.bAllowThrottleWhenFrameRateIsLow = false;
			InvalidOperation.Hyperlink = FSimpleDelegate::CreateLambda([]()
			{
				FPlatformProcess::LaunchURL(*MounteaInventoryEquipmentConsts::Hyperlinks::EquipmentConfigURL, nullptr, nullptr);
			});
			InvalidOperation.HyperlinkText = LOCTEXT("MounteaEquipmentConfigData_DuplicityLink", "Open Documentation");
		}
			
		FSlateNotificationManager::Get().AddNotification(InvalidOperation);
	}

#endif
	
	return bSatisfied;
	
}

#undef LOCTEXT_NAMESPACE
