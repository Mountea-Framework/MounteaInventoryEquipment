// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "MounteaAttachableComponent.generated.h"

enum class EAttachmentSlotState : uint8;
class IMounteaAdvancedAttachmentContainerInterface;

/**
 * Component that represents an attachable item in the Mountea Advanced Inventory System.
 * This component can be used to define items that can be attached to other actors or components.
 * It holds properties such as ID, display name, tags, state, and slot ID.
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,
	AutoExpandCategories=("Mountea","Attachable","Mountea|Attachable"),
	HideCategories=("Cooking","Collision"),
	meta=(BlueprintSpawnableComponent, DisplayName="Mountea Attachable Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMounteaAttachableComponent();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mountea")
	TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> AttachedTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mountea|Attachable")
	EAttachmentSlotState State;

public:
	
	bool IsValidAttachable() const;
	bool IsEmpty() const;
	bool IsOccupied() const;
	bool IsLocked() const;
	bool CanAttach() const;
	bool AttachTo(TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> Target, const FName& SlotId);
	bool AttachTo(TScriptInterface<IMounteaAdvancedAttachmentContainerInterface> Target);
	bool Detach();
	void Disable();
	bool HasTag(const FGameplayTag& Tag) const;
	bool MatchesTags(const FGameplayTagContainer& OtherTags, bool bRequireAll) const;
};
