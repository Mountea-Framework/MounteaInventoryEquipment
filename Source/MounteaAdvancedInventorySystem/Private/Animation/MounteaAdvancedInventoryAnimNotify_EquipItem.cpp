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


#include "Animation/MounteaAdvancedInventoryAnimNotify_EquipItem.h"

#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/Equipment/MounteaAdvancedEquipmentInterface.h"
#include "Statics/MounteaEquipmentStatics.h"

bool UMounteaAdvancedInventoryAnimNotify_EquipItem::Received_Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference) const
{
	(void)Animation;
	(void)EventReference;

	if (!IsValid(MeshComp))
		return false;

	UObject* interfaceSource = MeshComp->GetOwner();
	if (!IsValid(interfaceSource))
		interfaceSource = MeshComp;

	const TScriptInterface<IMounteaAdvancedEquipmentInterface> equipmentInterface =
		UMounteaEquipmentStatics::FindEquipmentInterface(interfaceSource);
	if (!equipmentInterface.GetObject())
		return false;

	return UMounteaEquipmentStatics::AnimAttachItem(equipmentInterface);
}
