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


#include "Statics/MounteaAttachmentsStatics.h"

#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
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
	TArray<USceneComponent*> components = GetAvailableComponents(Target);

	TArray<FName> names;
	names.Reserve(components.Num());

	Algo::TransformIf(
		components,
		names,
		[](const USceneComponent* Comp) { return IsValid(Comp); },
		[](const USceneComponent* Comp) { return Comp->GetFName(); }
	);

	return names;
}

USceneComponent* UMounteaAttachmentsStatics::GetAvailableComponentByName(const AActor* Target,
	const FName& ComponentName)
{
	if (!IsValid(Target) || ComponentName.IsNone())
		return nullptr;

	const TArray<USceneComponent*> components = GetAvailableComponents(Target);

	USceneComponent* const* match = Algo::FindByPredicate(
		components,
		[&](const USceneComponent* Comp)
		{
			return IsValid(Comp) && Comp->GetFName() == ComponentName;
		}
	);

	return match ? *match : nullptr;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableSocketNames(const AActor* Target, const FName& ComponentName)
{
	if (!IsValid(Target) || ComponentName.IsNone())
		return {};

	const TArray<USceneComponent*> components = GetAvailableComponents(Target);

	const USceneComponent* const* match = Algo::FindByPredicate(
		components,
		[&](const USceneComponent* Comp)
		{
			return IsValid(Comp) && Comp->GetFName() == ComponentName;
		}
	);

	return match ? (*match)->GetAllSocketNames() : TArray<FName>();
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

bool UMounteaAttachmentsStatics::TryAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_TryAttach(Target.GetObject(), SlotId, Attachment) : false;
}

bool UMounteaAttachmentsStatics::TryDetach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_TryDetach(Target.GetObject(), SlotId) : false;
}

bool UMounteaAttachmentsStatics::ForceAttach(const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target, const FName& SlotId, UObject* Attachment)
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

FName UMounteaAttachmentsStatics::GetFirstEmptySlot(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
		return Target.GetObject() != nullptr ? Target->Execute_GetFirstEmptySlot(Target.GetObject()) : NAME_None;
}

TArray<UMounteaAdvancedAttachmentSlot*> UMounteaAttachmentsStatics::GetAttachmentSlots(
	const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Target)
{
	return Target.GetObject() != nullptr ? Target->Execute_GetAttachmentSlots(Target.GetObject()) : TArray<UMounteaAdvancedAttachmentSlot*>();
}

TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> UMounteaAttachmentsStatics::GetAttachedTo(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetAttachedTo(Target.GetObject()) : TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>();
}

FName UMounteaAttachmentsStatics::GetId(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetId(Target.GetObject()) : NAME_None;
}

void UMounteaAttachmentsStatics::SetId(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FName& NewId)
{
	if (Target.GetObject() != nullptr)
		Target->Execute_SetId(Target.GetObject(), NewId);
}

FText UMounteaAttachmentsStatics::GetDisplayName(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetDisplayName(Target.GetObject()) : FText::GetEmpty();
}

void UMounteaAttachmentsStatics::SetDisplayName(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FText& NewDisplayName)
{
	if (Target.GetObject() != nullptr)
		Target->Execute_SetDisplayName(Target.GetObject(), NewDisplayName);
}

FGameplayTagContainer UMounteaAttachmentsStatics::GetTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetTags(Target.GetObject()) : FGameplayTagContainer();
}

void UMounteaAttachmentsStatics::SetTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTagContainer& NewTags)
{
	if (Target.GetObject() != nullptr)
		Target->Execute_SetTags(Target.GetObject(), NewTags);
}

EAttachmentState UMounteaAttachmentsStatics::GetState(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_GetState(Target.GetObject()) : EAttachmentState();
}

void UMounteaAttachmentsStatics::SetState(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const EAttachmentState NewState)
{
	if (Target.GetObject() != nullptr)
		Target->Execute_SetState(Target.GetObject(), NewState);
}

bool UMounteaAttachmentsStatics::IsValidAttachable(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_IsValidAttachable(Target.GetObject()) : false;
}

bool UMounteaAttachmentsStatics::CanAttach(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_CanAttach(Target.GetObject()) : false;
}

bool UMounteaAttachmentsStatics::AttachToSlot(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Container, const FName& SlotId)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_AttachToSlot(Target.GetObject(), Container, SlotId) : false;
}

bool UMounteaAttachmentsStatics::AttachToContainer(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const TScriptInterface<IMounteaAdvancedAttachmentContainerInterface>& Container)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_AttachToContainer(Target.GetObject(), Container) : false;
}

bool UMounteaAttachmentsStatics::Detach(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_Detach(Target.GetObject()) : false;
}

bool UMounteaAttachmentsStatics::HasTag(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTag& Tag)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_HasTag(Target.GetObject(), Tag) : false;
}

bool UMounteaAttachmentsStatics::MatchesTags(const TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface>& Target, const FGameplayTagContainer& OtherTags, bool bRequireAll)
{
	return (Target.GetObject() != nullptr) ? Target->Execute_MatchesTags(Target.GetObject(), OtherTags, bRequireAll) : false;
}