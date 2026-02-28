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

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MounteaAdvancedInventoryAnimNotify_EquipItem.generated.h"

/**
 * A specialized animation notify class designed for item equip actions in the Mountea Advanced Inventory System.
 *
 * This class triggers deferred equipment attach transitions from animation events.
 *
 * @note This class is part of the Mountea Advanced Inventory System module.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Equipment","Mountea|Equipment"),
	meta=(DisplayName="Mountea Equipment - Item Equip"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryAnimNotify_EquipItem : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

protected:
	static void ExecuteEquipAttach(USkeletalMeshComponent* MeshComp);
};
