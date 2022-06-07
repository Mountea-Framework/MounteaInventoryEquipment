// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Helpers/ActorInventoryBPFLibrary.h"

#include "Components/ActorInventoryManagerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

UActorInventoryManagerComponent* UActorInventoryBPFLibrary::GetInventoryManager(const UObject* WorldContextObject)
{
	if (const AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject))
	{
		if (UActorInventoryManagerComponent* InventoryManager = Cast<UActorInventoryManagerComponent>(GameState->GetComponentByClass(UActorInventoryManagerComponent::StaticClass())))
		{
			return InventoryManager;
		}
	}
	
	return nullptr;
}
