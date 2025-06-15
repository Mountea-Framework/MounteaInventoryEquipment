// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaAttachmentsStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAttachmentsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Returns an array of mesh components available on the target actor.
	 * 
	 * @param Target  Actor to get components from
	 * @return  Array of mesh components available on the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static TArray<USceneComponent*> GetAvailableComponents(const AActor* Target);

	/**
	 * Returns an array of names of mesh components available on the target actor.
	 * 
	 * @param Target  Actor to get component names from
	 * @return  Array of names of mesh components available on the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static TArray<FName> GetAvailableComponentNames(const AActor* Target);

	/**
	 * Returns an array of socket names available on the specified component of the target actor.
	 * 
	 * @param Target  Actor to get socket names from
	 * @param ComponentName  Name of the component to get socket names from
	 * @return  Array of socket names available on the specified component of the target actor.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mountea|Inventory & Equipment|Attachments|Helpers", meta=(CustomTag="MounteaK2Getter"))
	static TArray<FName> GetAvailableSocketNames(const AActor* Target, const FName& ComponentName);
};
