// All rights reserved Dominik Morse 2024


#include "Statics/MounteaAttachmentsStatics.h"

TArray<UMeshComponent*> UMounteaAttachmentsStatics::GetAvailableComponents(const AActor* Target)
{
	if (!IsValid(Target))
		return {};

	TArray<UMeshComponent*> components;
	Target->GetComponents<UMeshComponent>(components);
	return components;
}

TArray<FName> UMounteaAttachmentsStatics::GetAvailableComponentNames(const AActor* Target)
{
	TArray<FName> names;
	TArray<UMeshComponent*> components = GetAvailableComponents(Target);

	for (UMeshComponent* comp : components)
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

	TArray<UMeshComponent*> components = GetAvailableComponents(Target);
	for (UMeshComponent* comp : components)
	{
		if (IsValid(comp) && comp->GetFName() == ComponentName)
		{
			return comp->GetAllSocketNames();
		}
	}

	return TArray<FName>();
}
