﻿// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#endif

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

/**
 * Template that allows reading Name value from any given UENUM.
 * @param Name: Name of the UENUM (ECollisionChannel, for instance)
 * @param Value: Enum value to be translated (ECC_Visibility, for instance)
 * @return FString of the Value (ECC_Visibility in our example, or invalid of name not specified nor UENUM does not exist)
 */
template<typename TEnum>
static FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr) return FString("invalid");
	return enumPtr->GetDisplayNameTextByValue((int64)Value).ToString();
}

#if WITH_EDITOR

template <typename Source>
FString GetFilePath(Source* Asset)
{
	if (Asset == nullptr) return FString("");
	
	FString FileName = "/";
	FileName.Append(Asset->GetName());

	FString PackagePath = Asset->GetOutermost()->GetPackage()->GetLoadedPath().GetPackageFName().ToString();
	PackagePath.ReplaceInline((TEXT("%s"), *FileName), TEXT(""));

	return PackagePath;
}


template <class AssetType>
AssetType* CreateNewAsset(const FString& SaveObjectPath)
{
	AssetType* NewAsset = nullptr;

	if (!SaveObjectPath.IsEmpty())
	{
		// Attempt to load existing asset first
		NewAsset = FindObject<AssetType>(nullptr, *SaveObjectPath);

		// Proceed to creating a new asset, if needed
		if (!NewAsset)
		{
			const FString PackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
			const FString ObjectName = FPackageName::ObjectPathToObjectName(SaveObjectPath);

			NewAsset = NewObject<AssetType>(CreatePackage(*PackageName), AssetType::StaticClass(), FName(*ObjectName), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

			FAssetRegistryModule::AssetCreated(NewAsset);
			NewAsset->MarkPackageDirty();
		}       
	}

	return NewAsset;
}

#endif
