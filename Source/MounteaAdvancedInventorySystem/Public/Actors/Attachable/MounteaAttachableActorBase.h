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
#include "MounteaAttachableActorBase.generated.h"

/**
 * AMounteaAttachableActorBase provides a base actor class with built-in attachment functionality.
 * Attachable actors contain an attachable component that enables them to be attached to
 * attachment containers, providing a foundation for equipment items, weapons, and other attachable objects.
 *
 * @see [Attachment System](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/AttachmentSystem)
 * @see UMounteaAttachableComponent
 * @see UMounteaAttachmentContainerComponent
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Attachment System","Mountea|Attachment System"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Attachable Actor Base"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API AMounteaAttachableActorBase : public AActor
{
	GENERATED_BODY()

public:
	AMounteaAttachableActorBase();

protected:
	virtual void BeginPlay() override;

protected:

	/**
	 * Attachable Component responsible for attaching this Actor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment System")
	TObjectPtr<class UMounteaAttachableComponent> AttachableComponent;
};
