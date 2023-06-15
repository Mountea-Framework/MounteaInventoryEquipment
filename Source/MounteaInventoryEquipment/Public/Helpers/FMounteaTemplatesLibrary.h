// All rights reserved Dominik Pavlicek 2022.

#pragma once
#include "Engine/DataTable.h"

template<typename Name>
static TArray<Name> GetSourceRows(UDataTable* Table)
{
	if (!Table) return TArray<Name>();

	return Table->GetRowNames();
}

template<class Interface, class InterfaceClass>
static InterfaceClass GetInterfaceFrom(AActor* From)
{
	if (From == nullptr) return nullptr;

	if (From->Implements<Interface>()) return From;

	TArray<UActorComponent*> Components;
	From->GetComponents(Components);

	for (auto Itr : Components)
	{
		if (Itr && Itr->Implements<Interface>())
		{
			return Itr;
		}
	}
	return nullptr;
}