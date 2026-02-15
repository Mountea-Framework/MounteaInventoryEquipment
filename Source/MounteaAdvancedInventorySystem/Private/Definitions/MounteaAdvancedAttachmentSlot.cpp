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

#include "Definitions/MounteaAdvancedAttachmentSlot.h"

#include "Definitions/MounteaEquipmentBaseEnums.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentAttachableInterface.h"
#include "Interfaces/Attachments/MounteaAdvancedAttachmentContainerInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"
#include "Misc/DataValidation.h"
#include "Statics/MounteaAttachmentsStatics.h"

UMounteaAdvancedAttachmentSlot::UMounteaAdvancedAttachmentSlot()
{
	SlotName = "NewEmptySlot";
}

void UMounteaAdvancedAttachmentSlot::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	TryResolveAttachmentTarget();
}

void UMounteaAdvancedAttachmentSlot::TryResolveAttachmentTarget()
{
	if (AttachmentTargetOverride.IsNone() || !ParentContainer.GetObject())
		return;

	AActor* parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (IsValid(parentActor))
		AttachmentTargetComponentOverride = UMounteaAttachmentsStatics::GetAvailableComponentByName(parentActor, AttachmentTargetOverride);
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableSocketNames() const
{
	if (SlotType != EAttachmentSlotType::EAST_Socket || !IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	AActor* parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (!IsValid(parentActor))
		return TArray<FName>();

	FName targetName = AttachmentTargetOverride.IsNone() ? 
		ParentContainer->Execute_GetDefaultAttachmentTarget(ParentContainer.GetObject()) : 
		AttachmentTargetOverride;
		
	TArray<FName> returnValue = { NAME_None };
	returnValue.Append(UMounteaAttachmentsStatics::GetAvailableSocketNames(parentActor, targetName));
	return returnValue;
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableTargetNames() const
{
	if (!IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	AActor* parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	TArray<FName> returnValue = { NAME_None };
	if (IsValid(parentActor))
		returnValue.Append(UMounteaAttachmentsStatics::GetAvailableComponentNames(parentActor));
	return returnValue;
}

USceneComponent* UMounteaAdvancedAttachmentSlot::GetAttachmentTargetComponent() const
{
	return AttachmentTargetComponentOverride ? 
		AttachmentTargetComponentOverride : 
		TObjectPtr<USceneComponent>(ParentContainer->Execute_GetAttachmentTargetComponent(ParentContainer.GetObject()));
}

bool UMounteaAdvancedAttachmentSlot::PerformAttachmentLogic(UObject* NewAttachment)
{
	USceneComponent* attachmentTarget = GetAttachmentTargetComponent();
	if (!IsValid(attachmentTarget))
	{
		LOG_WARNING(TEXT("Attachment target component is invalid!"));
		return false;
	}

	if (!ValidateAttachmentSlot(attachmentTarget))
		return false;

	if (!PerformPhysicalAttachment(NewAttachment, attachmentTarget))
		return false;
	
	LastAttachment = Attachment;
	Attachment = NewAttachment;	
	State = EAttachmentSlotState::EASS_Occupied;
		
	HandleAttachableInterface(NewAttachment);
	return true;
}

void UMounteaAdvancedAttachmentSlot::HandleAttachableInterface(UObject* NewAttachment) const
{
	TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> attachableInterface = FindAttachableInterface(NewAttachment);
	if (attachableInterface.GetObject())
		IMounteaAdvancedAttachmentAttachableInterface::Execute_AttachToSlot(attachableInterface.GetObject(), ParentContainer, SlotName);
	else
		LOG_WARNING(TEXT("Attachment does not implement the attachable interface! Attachment will be performed, however, it may not behave as expected."));
}

bool UMounteaAdvancedAttachmentSlot::Attach(UObject* NewAttachment)
{
	return Super::Attach(NewAttachment) ? PerformAttachmentLogic(NewAttachment) : false;
}

bool UMounteaAdvancedAttachmentSlot::ForceAttach(UObject* NewAttachment)
{
	if (!IsValid(NewAttachment))
		return false;

	return PerformAttachmentLogic(NewAttachment);
}

TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> UMounteaAdvancedAttachmentSlot::FindAttachableInterface(UObject* Object)
{
	TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> returnAttachable;

	if (!IsValid(Object))
		return returnAttachable;

	if (Object->GetClass()->ImplementsInterface(UMounteaAdvancedAttachmentAttachableInterface::StaticClass()))
	{
		returnAttachable.SetObject(Object);
		returnAttachable.SetInterface(Cast<IMounteaAdvancedAttachmentAttachableInterface>(Object));
		return returnAttachable;
	}

	const AActor* targetActor = Cast<AActor>(Object);
	if (!IsValid(targetActor))
		return returnAttachable;

	const auto actorComponents = UMounteaAttachmentsStatics::GetAvailableComponents(targetActor);
	auto foundComponent = Algo::FindByPredicate(actorComponents, [](const UActorComponent* Comp)
	{
		return IsValid(Comp) && Comp->GetClass()->ImplementsInterface(UMounteaAdvancedAttachmentAttachableInterface::StaticClass());
	});

	if (foundComponent)
		returnAttachable = *foundComponent;

	return returnAttachable;
}

bool UMounteaAdvancedAttachmentSlot::ValidateAttachmentSlot(const USceneComponent* Target) const
{
	if (SlotType == EAttachmentSlotType::EAST_Socket && !Target->DoesSocketExist(SocketName))
	{
		LOG_WARNING(TEXT("Socket '%s' does not exist on target component '%s'!"), *SocketName.ToString(), *Target->GetName());
		return false;
	}
	return true;
}

bool UMounteaAdvancedAttachmentSlot::PerformPhysicalAttachment(UObject* Object, USceneComponent* Target) const
{
	const FAttachmentTransformRules attachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	const FName attachmentName = GetAttachmentSocketName();
	
	if (USceneComponent* sceneComp = Cast<USceneComponent>(Object))
		return sceneComp->AttachToComponent(Target, attachmentRules, attachmentName);
	
	if (AActor* actor = Cast<AActor>(Object))
		return actor->AttachToComponent(Target, attachmentRules, attachmentName);
	
	LOG_WARNING(TEXT("Unsupported attachment object type: %s"), *Object->GetName());
	return false;
}

FName UMounteaAdvancedAttachmentSlot::GetAttachmentSocketName() const
{
	return SlotType == EAttachmentSlotType::EAST_Socket ? SocketName : NAME_None;
}

bool UMounteaAdvancedAttachmentSlot::Detach()
{
	if (!CanDetach())
		return false;

	PerformDetachment();
	return Super::Detach();
}

bool UMounteaAdvancedAttachmentSlot::ForceDetach()
{
	PerformDetachment();
	return Super::ForceDetach();
}

void UMounteaAdvancedAttachmentSlot::PerformDetachment() const
{
	UObject* targetAttachment = Attachment ? Attachment : LastAttachment;
	
	if (USceneComponent* sceneComp = Cast<USceneComponent>(targetAttachment))
		sceneComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	else if (AActor* actor = Cast<AActor>(targetAttachment))
		actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (IsValid(targetAttachment) && targetAttachment->Implements<UMounteaAdvancedAttachmentAttachableInterface>())
		IMounteaAdvancedAttachmentAttachableInterface::Execute_SetState(targetAttachment, EAttachmentState::EAS_Detached);
}

void UMounteaAdvancedAttachmentSlot::OnRep_State()
{
	switch (State)
	{
		case EAttachmentSlotState::EASS_Occupied:
			if (Attachment)
			{
				TryResolveAttachmentTarget();
				if (USceneComponent* attachmentTarget = GetAttachmentTargetComponent())
				{
					if (ValidateAttachmentSlot(attachmentTarget))
						PerformPhysicalAttachment(Attachment, attachmentTarget);
				}
			}
			break;
		case EAttachmentSlotState::EASS_Empty:
			PerformDetachment();
			break;
	}
	
	Super::OnRep_State();
}

#if WITH_EDITOR
EDataValidationResult UMounteaAdvancedAttachmentSlot::IsDataValid(class FDataValidationContext& Context) const
{
	if (!IsSlotValid())
		Context.AddError(NSLOCTEXT("AttachmentSlot", "AttachmentSlot_SlotInvalid", "Slot is invalid! It must have a valid name and tags."));
	
	UObject::IsDataValid(Context);
	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
