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

#include "Helpers/MounteaJsonDefinitionDependencyScanner.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Async/Async.h"
#include "Containers/Ticker.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Editor.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformTime.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Settings/MounteaAdvancedInventoryGlobalConfig.h"
#include "SourceCodeNavigation.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "MounteaJsonDefinitionDependencyScanner"

namespace MounteaJsonDefinitionDependencyScannerStatics
{
	using FUsageMap = TMap<FString, TArray<FMounteaJsonDefinitionUsage>>;

	static const FName DefinitionPinName(TEXT("DefinitionKey"));
	static constexpr double ScanTimeBudgetSeconds = 0.008;

	static FDelegateHandle RecalculateHandle;

	static TMap<FString, TArray<FMounteaJsonDefinitionUsage>> CachedUsages;
	static FString CachedScanTime;
	static bool bCacheLoaded = false;

	struct FScanState
	{
		TArray<FAssetData> Blueprints;
		int32 NextBlueprint = 0;
		TSet<FString> Keys;
		FUsageMap Results;
		TFuture<FUsageMap> SourceScan;
		TSharedPtr<SNotificationItem> Notification;
		bool bCancelled = false;
	};

	static TSharedPtr<FScanState> ActiveScan;

	static FString GetStorePath()
	{
		return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MounteaInventory"), TEXT("JsonDefinitionDependencies.json"));
	}

	static void AddUsage(FUsageMap& Results, const FString& Key, const FString& Source, const FString& Path, const int32 Count)
	{
		TArray<FMounteaJsonDefinitionUsage>& usages = Results.FindOrAdd(Key);
		FMounteaJsonDefinitionUsage* existing = usages.FindByPredicate([&Path](const FMounteaJsonDefinitionUsage& usage)
		{
			return usage.Path == Path;
		});

		if (!existing)
		{
			existing = &usages.AddDefaulted_GetRef();
			existing->Source = Source;
			existing->Path = Path;
		}

		existing->Count += Count;
	}

	static void MergeUsages(FUsageMap& Target, const FUsageMap& Source)
	{
		for (const TPair<FString, TArray<FMounteaJsonDefinitionUsage>>& pair : Source)
		{
			for (const FMounteaJsonDefinitionUsage& usage : pair.Value)
			{
				AddUsage(Target, pair.Key, usage.Source, usage.Path, usage.Count);
			}
		}
	}

	static void CountBlueprintUsages(UBlueprint* Blueprint, const TSet<FString>& Keys, FUsageMap& Results)
	{
		TArray<UEdGraph*> graphs;
		Blueprint->GetAllGraphs(graphs);

		const FString path = Blueprint->GetPathName();
		for (const UEdGraph* graph : graphs)
		{
			if (!graph)
			{
				continue;
			}

			for (const UEdGraphNode* node : graph->Nodes)
			{
				if (!node)
				{
					continue;
				}

				for (const UEdGraphPin* pin : node->Pins)
				{
					if (!pin || pin->Direction != EGPD_Input || pin->PinName != DefinitionPinName)
					{
						continue;
					}

					if (Keys.Contains(pin->DefaultValue))
					{
						AddUsage(Results, pin->DefaultValue, TEXT("Blueprint"), path, 1);
					}
				}
			}
		}
	}

	static FUsageMap ScanSourceFiles(const TArray<FString>& SourceDirs, const TArray<FString>& Keys)
	{
		FUsageMap results;

		TArray<FString> files;
		for (const FString& dir : SourceDirs)
		{
			TArray<FString> found;
			IFileManager::Get().FindFilesRecursive(found, *dir, TEXT("*.h"), true, false);
			files.Append(found);

			found.Reset();
			IFileManager::Get().FindFilesRecursive(found, *dir, TEXT("*.cpp"), true, false);
			files.Append(found);
		}

		for (const FString& file : files)
		{
			FString content;
			if (!FFileHelper::LoadFileToString(content, *file))
			{
				continue;
			}

			for (const FString& key : Keys)
			{
				const int32 count = FMounteaJsonDefinitionDependencyScanner::CountKeyLiterals(content, key);
				if (count > 0)
				{
					AddUsage(results, key, TEXT("C++"), file, count);
				}
			}
		}

		return results;
	}

	static void SaveStore()
	{
		TSharedRef<FJsonObject> root = MakeShared<FJsonObject>();
		root->SetStringField(TEXT("generatedAt"), CachedScanTime);

		TSharedRef<FJsonObject> definitions = MakeShared<FJsonObject>();
		for (const TPair<FString, TArray<FMounteaJsonDefinitionUsage>>& pair : CachedUsages)
		{
			TArray<TSharedPtr<FJsonValue>> entries;
			for (const FMounteaJsonDefinitionUsage& usage : pair.Value)
			{
				TSharedRef<FJsonObject> entry = MakeShared<FJsonObject>();
				entry->SetStringField(TEXT("source"), usage.Source);
				entry->SetStringField(TEXT("path"), usage.Path);
				entry->SetNumberField(TEXT("count"), usage.Count);
				entries.Add(MakeShared<FJsonValueObject>(entry));
			}

			definitions->SetArrayField(pair.Key, entries);
		}
		root->SetObjectField(TEXT("definitions"), definitions);

		FString output;
		TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&output);
		FJsonSerializer::Serialize(root, writer);

		const FString storePath = GetStorePath();
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(storePath), true);
		FFileHelper::SaveStringToFile(output, *storePath);
	}

	static void EnsureCacheLoaded()
	{
		if (bCacheLoaded)
		{
			return;
		}

		bCacheLoaded = true;

		FString json;
		if (!FFileHelper::LoadFileToString(json, *GetStorePath()))
		{
			return;
		}

		TSharedPtr<FJsonObject> root;
		const TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json);
		if (!FJsonSerializer::Deserialize(reader, root) || !root.IsValid())
		{
			return;
		}

		root->TryGetStringField(TEXT("generatedAt"), CachedScanTime);

		const TSharedPtr<FJsonObject>* definitions = nullptr;
		if (!root->TryGetObjectField(TEXT("definitions"), definitions))
		{
			return;
		}

		for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : (*definitions)->Values)
		{
			const TArray<TSharedPtr<FJsonValue>>* entries = nullptr;
			if (!pair.Value.IsValid() || !pair.Value->TryGetArray(entries))
			{
				continue;
			}

			TArray<FMounteaJsonDefinitionUsage>& usages = CachedUsages.Add(pair.Key);
			for (const TSharedPtr<FJsonValue>& entryValue : *entries)
			{
				const TSharedPtr<FJsonObject>* entry = nullptr;
				if (!entryValue.IsValid() || !entryValue->TryGetObject(entry))
				{
					continue;
				}

				FMounteaJsonDefinitionUsage usage;
				double count = 0.0;
				(*entry)->TryGetStringField(TEXT("source"), usage.Source);
				(*entry)->TryGetStringField(TEXT("path"), usage.Path);
				(*entry)->TryGetNumberField(TEXT("count"), count);
				usage.Count = static_cast<int32>(count);
				usages.Add(usage);
			}
		}
	}

	static void CompleteScan(const TSharedPtr<FScanState>& State)
	{
		FUsageMap merged = State->Results;
		MergeUsages(merged, State->SourceScan.Get());

		bCacheLoaded = true;
		CachedUsages = merged;
		CachedScanTime = FDateTime::Now().ToString();
		SaveStore();

		if (State->Notification.IsValid())
		{
			State->Notification->SetText(FText::Format(
				LOCTEXT("ScanDone", "Dependency scan complete: {0} definition(s) are in use."),
				FText::AsNumber(CachedUsages.Num())));
			State->Notification->SetCompletionState(SNotificationItem::CS_Success);
			State->Notification->ExpireAndFadeout();
		}

		ActiveScan.Reset();
	}

	static void CancelScan(const TSharedPtr<FScanState>& State)
	{
		if (State->Notification.IsValid())
		{
			State->Notification->SetText(LOCTEXT("ScanCancelled", "Dependency scan cancelled. Previous results were kept."));
			State->Notification->SetCompletionState(SNotificationItem::CS_Fail);
			State->Notification->ExpireAndFadeout();
		}

		ActiveScan.Reset();
	}

	static bool TickScan(const TSharedPtr<FScanState>& State)
	{
		const double deadline = FPlatformTime::Seconds() + ScanTimeBudgetSeconds;
		while (!State->bCancelled && State->NextBlueprint < State->Blueprints.Num() && FPlatformTime::Seconds() < deadline)
		{
			const FAssetData& assetData = State->Blueprints[State->NextBlueprint];
			State->NextBlueprint++;

			if (UBlueprint* blueprint = Cast<UBlueprint>(assetData.GetAsset()))
			{
				CountBlueprintUsages(blueprint, State->Keys, State->Results);
			}
		}

		if (State->bCancelled)
		{
			CancelScan(State);
			return false;
		}

		if (State->NextBlueprint < State->Blueprints.Num() || !State->SourceScan.IsReady())
		{
			return true;
		}

		CompleteScan(State);
		return false;
	}

	static void HandleRecalculateRequest(UMounteaAdvancedInventoryGlobalConfig* Config)
	{
		if (!Config)
		{
			return;
		}

		TArray<FString> keys;
		Config->JsonObjectDefinitions.GetKeys(keys);
		FMounteaJsonDefinitionDependencyScanner::StartScan(keys);
	}
}

using namespace MounteaJsonDefinitionDependencyScannerStatics;

void FMounteaJsonDefinitionDependencyScanner::Bind()
{
	RecalculateHandle = UMounteaAdvancedInventoryGlobalConfig::OnRecalculateDependencies.AddStatic(&HandleRecalculateRequest);
}

void FMounteaJsonDefinitionDependencyScanner::Unbind()
{
	UMounteaAdvancedInventoryGlobalConfig::OnRecalculateDependencies.Remove(RecalculateHandle);
	RecalculateHandle.Reset();
}

void FMounteaJsonDefinitionDependencyScanner::StartScan(const TArray<FString>& DefinitionKeys)
{
	if (ActiveScan.IsValid())
	{
		return;
	}

	const TSharedPtr<FScanState> state = MakeShared<FScanState>();
	state->Keys = TSet<FString>(DefinitionKeys);

	FAssetRegistryModule& registryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	registryModule.Get().SearchAllAssets(true);
	registryModule.Get().GetAssetsByClass(UBlueprint::StaticClass()->GetClassPathName(), state->Blueprints, true);

	TArray<FString> sourceDirs;
	if (const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem")))
	{
		sourceDirs.Add(FPaths::Combine(plugin->GetBaseDir(), TEXT("Source")));
	}
	sourceDirs.Add(FPaths::GameSourceDir());

	const TArray<FString> keys = state->Keys.Array();
	state->SourceScan = Async(EAsyncExecution::ThreadPool, [sourceDirs, keys]()
	{
		return ScanSourceFiles(sourceDirs, keys);
	});

	const TWeakPtr<FScanState> weakState = state;
	FNotificationInfo info(LOCTEXT("ScanStarted", "Scanning JSON definition dependencies..."));
	info.bFireAndForget = false;
	info.ButtonDetails.Add(FNotificationButtonInfo(
		LOCTEXT("CancelScan", "Cancel"),
		FText::GetEmpty(),
		FSimpleDelegate::CreateLambda([weakState]()
		{
			if (const TSharedPtr<FScanState> pinned = weakState.Pin())
			{
				pinned->bCancelled = true;
			}
		})));

	state->Notification = FSlateNotificationManager::Get().AddNotification(info);
	state->Notification->SetCompletionState(SNotificationItem::CS_Pending);

	ActiveScan = state;
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([state](float)
	{
		return TickScan(state);
	}));
}

TArray<FMounteaJsonDefinitionUsage> FMounteaJsonDefinitionDependencyScanner::GetUsages(const FString& DefinitionKey)
{
	EnsureCacheLoaded();

	if (const TArray<FMounteaJsonDefinitionUsage>* usages = CachedUsages.Find(DefinitionKey))
	{
		return *usages;
	}

	return {};
}

FString FMounteaJsonDefinitionDependencyScanner::GetLastScanTime()
{
	EnsureCacheLoaded();
	return CachedScanTime;
}

int32 FMounteaJsonDefinitionDependencyScanner::CountKeyLiterals(const FString& Content, const FString& Key)
{
	if (Key.IsEmpty())
	{
		return 0;
	}

	const FString literal = FString::Printf(TEXT("\"%s\""), *Key);
	int32 count = 0;
	int32 index = Content.Find(literal, ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
	while (index != INDEX_NONE)
	{
		count++;
		index = Content.Find(literal, ESearchCase::CaseSensitive, ESearchDir::FromStart, index + literal.Len());
	}

	return count;
}

#undef LOCTEXT_NAMESPACE
