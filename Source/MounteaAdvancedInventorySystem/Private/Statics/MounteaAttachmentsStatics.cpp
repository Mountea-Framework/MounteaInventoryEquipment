// All rights reserved Dominik Morse 2024


#include "Statics/MounteaAttachmentsStatics.h"

TArray<USceneComponent*> UMounteaAttachmentsStatics::GetAvailableComponents(const AActor* Target)
{
	if (!IsValid(Target))
		return {};

	TArray<USceneComponent*> components;
	Target->GetComponents<USceneComponent>(components);
	return components;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableComponentNames(const AActor* Target)
{
	TArray<FName> names;
	TArray<USceneComponent*> components = GetAvailableComponents(Target);

	for (USceneComponent* comp : components)
	{
		if (IsValid(comp))
			names.Add(comp->GetFName());
	}

	return names;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableSocketNames(const AActor* Target, const FName& ComponentName)
{
	if (!IsValid(Target))
		return {};

	TArray<USceneComponent*> components = GetAvailableComponents(Target);
	for (USceneComponent* comp : components)
	{
		if (IsValid(comp) && comp->GetFName() == ComponentName)
			return comp->GetAllSocketNames();
	}

	return TArray<FName>();
}
