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

#include "CoreMinimal.h"

enum class EMAISSetupItemStatus : uint8
{
	AlreadyPresent,
	Added,
	NativeClass,
	Failed,
	Skipped
};

enum class EMAISSetupActorRole : uint8
{
	Settings,
	Player
};

struct FMAISSetupItemResult
{
	EMAISSetupActorRole Role = EMAISSetupActorRole::Settings;
	EMAISSetupItemStatus Status = EMAISSetupItemStatus::Skipped;
	FString ItemName;
	FString Message;

	static FMAISSetupItemResult Make(
		const EMAISSetupActorRole Role,
		const EMAISSetupItemStatus Status,
		const FString& ItemName,
		const FString& Message
	);
};

struct FMAISSetupDefaultsReport
{
	TArray<FMAISSetupItemResult> SettingsResults;
	TArray<FMAISSetupItemResult> PlayerResults;
	FString GameModeName;
	FString PlayerClassName;
	bool bSettingsSaved = false;
	bool bIsDefaultGameMode = false;

	bool HasFailures() const;
};

class FMounteaInventorySetupUtilities
{
public:

	static FMAISSetupDefaultsReport RunSetupDefaults();
	static FMAISSetupDefaultsReport ApplySettingsDefaults();

private:

	static void ApplySettingsDefaults(FMAISSetupDefaultsReport& Report);
	static void SetupPlayerComponents(FMAISSetupDefaultsReport& Report);
};
