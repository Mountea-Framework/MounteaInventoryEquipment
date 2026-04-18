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
#include "GameFramework/Actor.h"
#include "Interfaces/Equipment/MounteaAdvancedQuickUseItemInterface.h"
#include "MounteaEquipmentItemBase.generated.h"

class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * AMounteaEquipmentItemBase provides a base actor class with built-in equipment item functionality.
 * Equipment Item actors contain an attachable component that enables them to be attached to
 * attachment containers, providing a foundation for equipment items, weapons, and other attachable objects.
 *
 * @see [Equipment System](https://mountea.tools/docs/advancedinventoryequipmentsystem/equipmentsystem/equipmentsystem/)
 * @see UMounteaEquipmentItemComponent
 * @see UMounteaAttachableComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Equipment","Mountea|Equipment"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Equipment Item Base"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API AMounteaEquipmentItemBase : public AActor, public IMounteaAdvancedQuickUseItemInterface
{
	GENERATED_BODY()

public:
	AMounteaEquipmentItemBase();

public:

	virtual bool SetStaticMesh_Implementation(UStaticMesh* NewStaticMesh) override;
	virtual bool SetSkeletalMesh_Implementation(USkeletalMesh* NewSkeletalMesh) override;

protected:
	
	virtual void BeginPlay() override;
	
protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	/**
	 * Equipment Item Component responsible for attaching this Actor to Player/NPC.
	 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="Equipment")
	TObjectPtr<class UMounteaEquipmentItemComponent> EquipmentItemComponent;
};
