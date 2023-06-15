// All rights reserved Dominik Pavlicek 2022.

#pragma once

template<typename Name, class TableClass>
static TArray<Name> GetSourceRows(TableClass* Table)
{
	if (!Table) return TArray<Name>();

	return Table->GetRowNames();
}

template<class RowStruct, class TableClass>
static RowStruct* GetRow(FName RowName, TableClass* Table)
{
	if (!Table) return nullptr;

	const FString Context;
	return Table->template FindRow<RowStruct>(RowName, Context);
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

