// All rights reserved Dominik Morse 2024


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
	if (AttachmentTargetOverride.IsNone())
		return;

	UObject* parentContainerObj = ParentContainer.GetObject();
	if (!parentContainerObj)
		return;

	AActor* parentActor = ParentContainer->Execute_GetOwningActor(parentContainerObj);
	if (!IsValid(parentActor))
		return;

	AttachmentTargetComponentOverride = UMounteaAttachmentsStatics::GetAvailableComponentByName(
		parentActor, AttachmentTargetOverride);
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableSocketNames() const
{
	switch (SlotType)
	{		
		case EAttachmentSlotType::EAST_Socket:
			break;
		case EAttachmentSlotType::EAST_Component:
		case EAttachmentSlotType::Default:
			return TArray<FName>();
	}
	
	if (!IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	auto parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (!IsValid(parentActor))
		return TArray<FName>();

	return UMounteaAttachmentsStatics::GetAvailableSocketNames(parentActor,
		AttachmentTargetOverride.IsNone() ?
		ParentContainer->Execute_GetDefaultAttachmentTarget(ParentContainer.GetObject()) :
		AttachmentTargetOverride);
}

TArray<FName> UMounteaAdvancedAttachmentSlot::GetAvailableTargetNames() const
{
	if (!IsValid(ParentContainer.GetObject()))
		return TArray<FName>();

	auto parentActor = ParentContainer->Execute_GetOwningActor(ParentContainer.GetObject());
	if (!IsValid(parentActor))
		return TArray<FName>();

	return UMounteaAttachmentsStatics::GetAvailableComponentNames(parentActor);
}

bool UMounteaAdvancedAttachmentSlot::IsEmpty() const
{
	return State == EAttachmentSlotState::EASS_Empty && Attachment == nullptr;
}

bool UMounteaAdvancedAttachmentSlot::IsOccupied() const
{
	return State == EAttachmentSlotState::EASS_Occupied && Attachment != nullptr;
}

void UMounteaAdvancedAttachmentSlot::DisableSlot()
{
	if (!IsEmpty())
		Detach();
	State = EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAdvancedAttachmentSlot::IsLocked() const
{
	return State == EAttachmentSlotState::EASS_Locked;
}

bool UMounteaAdvancedAttachmentSlot::Attach(UObject* NewAttachment)
{
	if (!CanAttach() || !IsValid(NewAttachment))
		return false;

	USceneComponent* attachmentTarget = AttachmentTargetComponentOverride ? 
		AttachmentTargetComponentOverride : 
		ParentContainer->Execute_GetAttachmentTargetComponent(ParentContainer.GetObject());
	if (!IsValid(attachmentTarget))
	{
		LOG_WARNING(TEXT("Attachment target component is invalid!"));
		return false;
	}

	if (!IsValidForAttachment(NewAttachment))
		return false;

	if (!ValidateAttachmentSlot(attachmentTarget))
		return false;

	if (!PerformPhysicalAttachment(NewAttachment, attachmentTarget))
		return false;

	Attachment = NewAttachment;
	State = EAttachmentSlotState::EASS_Occupied;
	
	TScriptInterface<IMounteaAdvancedAttachmentAttachableInterface> attachableInterface = FindAttachableInterface(NewAttachment);
	if (attachableInterface.GetObject())
		IMounteaAdvancedAttachmentAttachableInterface::Execute_AttachToSlot(
					attachableInterface.GetObject(), ParentContainer, SlotName);
	else
		LOG_WARNING(TEXT("Attachment does not implement the attachable interface! Attachment will be performed, however, it may not behave as expected."));
    
	return true;
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

	auto foundComponent = Algo::FindByPredicate(
		actorComponents,
		[](const UActorComponent* Comp)
		{
			return IsValid(Comp) &&
				Comp->GetClass()->ImplementsInterface(UMounteaAdvancedAttachmentAttachableInterface::StaticClass());
		}
	);

	if (foundComponent)
		returnAttachable = *foundComponent;

	return returnAttachable;
}

bool UMounteaAdvancedAttachmentSlot::IsValidForAttachment(const UObject* NewAttachment)
{
	if (!IsValid(NewAttachment))
	{
		LOG_ERROR(TEXT("Attachment is not valid!"));
		return false;
	}
	
	if (!NewAttachment->Implements<UMounteaAdvancedAttachmentAttachableInterface>())
	{
		LOG_WARNING(TEXT("Attachable %s does not implement the required interface! Attachment will be performed, however, it may not behave as expected."), *NewAttachment->GetName())
		return true;
	}

	if (!IMounteaAdvancedAttachmentAttachableInterface::Execute_CanAttach(NewAttachment))
	{
		LOG_WARNING(TEXT("Attachable object is not compatible with selected Slot!."));
		return false;
	}
	
	return true;
}

bool UMounteaAdvancedAttachmentSlot::ValidateAttachmentSlot(const USceneComponent* Target) const
{
	if (SlotType == EAttachmentSlotType::EAST_Socket && !Target->DoesSocketExist(SocketName))
	{
		LOG_WARNING(TEXT("Socket '%s' does not exist on target component '%s'!"),
			*SocketName.ToString(), *Target->GetName());
		return false;
	}
	return true;
}

bool UMounteaAdvancedAttachmentSlot::PerformPhysicalAttachment(UObject* Object, USceneComponent* Target) const
{
	const FAttachmentTransformRules attachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	const FName attachmentName = GetAttachmentSocketName();
	
	if (USceneComponent* sceneComp = Cast<USceneComponent>(Object))
	{
		sceneComp->AttachToComponent(Target, attachmentRules, attachmentName);
		return true;
	}
	
	if (AActor* actor = Cast<AActor>(Object))
	{
		actor->AttachToComponent(Target, attachmentRules, attachmentName);
		return true;
	}
	
	LOG_WARNING(TEXT("Unsupported attachment object type: %s"), *Object->GetName());
	return false;
}

FName UMounteaAdvancedAttachmentSlot::GetAttachmentSocketName() const
{
	switch (SlotType)
	{
		case EAttachmentSlotType::EAST_Socket:
			return SocketName;
		case EAttachmentSlotType::EAST_Component:
			return NAME_None;
		default:
			return NAME_None;
	}
}

bool UMounteaAdvancedAttachmentSlot::Detach()
{
	if (!IsOccupied())
		return false;

	PerformPhysicalDetachment();

	if (IsValid(Attachment) && Attachment->Implements<UMounteaAdvancedAttachmentAttachableInterface>())
		IMounteaAdvancedAttachmentAttachableInterface::Execute_SetState(Attachment, EAttachmentState::EAS_Detached);

	Attachment = nullptr;
	State = EAttachmentSlotState::EASS_Empty;
	return true;
}

void UMounteaAdvancedAttachmentSlot::PerformPhysicalDetachment() const
{
	if (USceneComponent* sceneComp = Cast<USceneComponent>(Attachment))
		sceneComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	else if (AActor* actor = Cast<AActor>(Attachment))
		actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

// TODO: How to handle detachments? We cannot call this as we have no idea if we are detaching or attaching...
void UMounteaAdvancedAttachmentSlot::OnRep_Attachment()
{
	USceneComponent* attachmentTarget = AttachmentTargetComponentOverride ? 
		AttachmentTargetComponentOverride : 
		ParentContainer->Execute_GetAttachmentTargetComponent(ParentContainer.GetObject());
	if (!IsValid(attachmentTarget))
	{
		LOG_WARNING(TEXT("Attachment target component is invalid!"));
		return;
	}

	if (!IsValidForAttachment(Attachment))
		return;

	if (!ValidateAttachmentSlot(attachmentTarget))
		return;

	if (!PerformPhysicalAttachment(Attachment, attachmentTarget))
		return;
	
	Super::OnRep_Attachment();
}

#if WITH_EDITOR

EDataValidationResult UMounteaAdvancedAttachmentSlot::IsDataValid(class FDataValidationContext& Context) const
{
	if (!IsSlotValid())
	{
		Context.AddError(NSLOCTEXT("AttachmentSlot", "AttachmentSlot_SlotInvalid", "Slot is invalid! It must have a valid name and tags."));
	}
	UObject::IsDataValid(Context);
	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif