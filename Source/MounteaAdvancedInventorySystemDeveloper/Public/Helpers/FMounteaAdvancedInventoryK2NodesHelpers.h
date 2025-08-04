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
#include "Interfaces/IPluginManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"

enum class EFunctionCallType
{
	Function,
	Message,
	Delegate,
	Unknown
};

enum class EFunctionRole
{
	Set,
	Get,
	Validate,
	Unknown
};

namespace MounteaAdvancedInventoryHelpers
{
	TSharedPtr<IPlugin> GetThisPlugin();
	const TSet<UClass*> GetRelevantClasses();

	inline TSharedPtr<IPlugin> GetThisPlugin()
	{
		return IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
	}

	inline const TSet<UClass*> GetRelevantClasses()
	{
		static TSet<UClass*> relevantClasses;
		
		if (!relevantClasses.IsEmpty())
		{
			return relevantClasses;
		}

		const TSharedPtr<IPlugin> thisPlugin = GetThisPlugin();
		if (!thisPlugin.IsValid())
		{
			return relevantClasses;
		}

		TSet<FString> thisPluginModulesNames;
		for (const FModuleDescriptor& module : thisPlugin->GetDescriptor().Modules)
		{
			thisPluginModulesNames.Add(module.Name.ToString());
		}
		
		for (TObjectIterator<UClass> it; it; ++it)
		{
			UClass* classType = *it;

			const FString packageName = FPackageName::GetLongPackageAssetName(classType->GetPackage()->GetName());
			if (thisPluginModulesNames.Contains(packageName))
			{
				// Include function libraries
				if (classType->IsChildOf(UBlueprintFunctionLibrary::StaticClass()))
				{
					relevantClasses.Add(classType);
				}
				// Include interfaces
				else if (classType->IsChildOf(UInterface::StaticClass()))
				{
					relevantClasses.Add(classType);
				}
				// Include regular classes
				else if (!classType->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
				{
					relevantClasses.Add(classType);
				}
			}
		}

		return relevantClasses;
	}
}