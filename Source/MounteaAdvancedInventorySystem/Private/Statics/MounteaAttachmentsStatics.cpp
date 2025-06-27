// All rights reserved Dominik Morse 2024


#include "Statics/MounteaAttachmentsStatics.h"

#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"

TArray<USceneComponent*> UMounteaAttachmentsStatics::GetAvailableComponents(const AActor* Target)
{
	if (!IsValid(Target))
		return {};

	TArray<USceneComponent*> components;
	Target->GetComponents<USceneComponent>(components);
	return components;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableComponentNames(const AActor* Target)
{
	TArray<FName> names;
	TArray<USceneComponent*> components = GetAvailableComponents(Target);

	for (USceneComponent* comp : components)
	{
		if (IsValid(comp))
			names.Add(comp->GetFName());
	}

	return names;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableSocketNames(const AActor* Target, const FName& ComponentName)
{
	if (!IsValid(Target))
		return {};

	TArray<USceneComponent*> components = GetAvailableComponents(Target);
	for (USceneComponent* comp : components)
	{
		if (IsValid(comp) && comp->GetFName() == ComponentName)
			return comp->GetAllSocketNames();
	}

	return TArray<FName>();
}

AActor* UMounteaAttachmentsStatics::GetOwningActor(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetOwningActor(Target.GetObject()) : nullptr;
}

FName UMounteaAttachmentsStatics::GetDefaultAttachmentTarget(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetDefaultAttachmentTarget(Target.GetObject()) : NAME_None;
}

bool UMounteaAttachmentsStatics::IsValidSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_IsValidSlot(Target.GetObject(), SlotId) : false;
}

UMounteaAdvancedAttachmentSlot* UMounteaAttachmentsStatics::GetSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetSlot(Target.GetObject(), SlotId) : nullptr;
}

bool UMounteaAttachmentsStatics::IsSlotOccupied(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_IsSlotOccupied(Target.GetObject(), SlotId) : false;
}

bool UMounteaAttachmentsStatics::DisableSlot(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_DisableSlot(Target.GetObject(), SlotId) : false;
}

bool UMounteaAttachmentsStatics::TryAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UMounteaAttachableComponent* Attachment)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_TryAttach(Target.GetObject(), SlotId, Attachment) : false;
}

bool UMounteaAttachmentsStatics::TryDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_TryDetach(Target.GetObject(), SlotId) : false;
}

bool UMounteaAttachmentsStatics::ForceAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UMounteaAttachableComponent* Attachment)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_ForceAttach(Target.GetObject(), SlotId, Attachment) : false;
}

bool UMounteaAttachmentsStatics::ForceDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_ForceDetach(Target.GetObject(), SlotId) : false;
}

FName UMounteaAttachmentsStatics::FindFirstFreeSlotWithTags(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FGameplayTagContainer& RequiredTags)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_FindFirstFreeSlotWithTags(Target.GetObject(), RequiredTags) : NAME_None;
}

FName UMounteaAttachmentsStatics::GetSlotIdForAttachable(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const UMounteaAttachableComponent* Attachable)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetSlotIdForAttachable(Target.GetObject(), Attachable) : NAME_None;
}

void UMounteaAttachmentsStatics::ClearAll(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
	if (Target.GetObject() != nullptr)
		Target->Execute_ClearAll(Target.GetObject());
}
