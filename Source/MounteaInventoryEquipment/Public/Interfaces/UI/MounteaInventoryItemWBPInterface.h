// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Helpers/MounteaItemHelpers.h"
#include "UObject/Interface.h"
#include "MounteaInventoryItemWBPInterface.generated.h"

class IMounteaInventoryWBPInterface;
class UMounteaBaseUserWidget;
class UMounteaInventoryItemBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryItemWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryItemWBPInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FItemSlot GetItem() const;
	virtual FItemSlot GetItem_Implementation() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	UMounteaBaseUserWidget* GetItemUI();
	virtual UMounteaBaseUserWidget* GetItemUI_Implementation() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FIntPoint GetRootCoords() const;
	virtual FIntPoint GetRootCoords_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	FIntPoint GetItemDimensions() const;
	virtual FIntPoint GetItemDimensions_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool SaveRootCoords(const FIntPoint& NewCoords);
	virtual bool SaveRootCoords_Implementation(const FIntPoint& NewCoords) = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool MoveToNewCoords(const FIntPoint& NewCoords);
	virtual bool MoveToNewCoords_Implementation(const FIntPoint& NewCoords) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool ReleaseOldCoords(const FIntPoint& OldCoords);
	virtual bool ReleaseOldCoords_Implementation(const FIntPoint& OldCoords) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool CanBeDragged() const;
	virtual bool CanBeDragged_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void OnDropped();
	virtual void OnDropped_Implementation() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory")
	void OnDragged();
	virtual void OnDragged_Implementation() = 0;
	
};
