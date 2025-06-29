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

#include "MounteaAdvancedInventoryLog.h"

#include "MounteaAdvancedInventorySystemLogTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

// Log category definition
DEFINE_LOG_CATEGORY(LogMounteaAdvancedInventorySystem);

void PrintLog(const ELogVerbosity::Type Verbosity, const FString& Message, FLinearColor Color, float Duration)
{
	if (!GWorld) return;

	bool isVerbosityAllowed = false;
	
	if (const UMounteaAdvancedInventorySettings* inventorySettings = GetDefault<UMounteaAdvancedInventorySettings>())
	{
		const EMounteaAdvancedInventoryLoggingVerbosity AllowedLogging = inventorySettings->GetAllowedLoggVerbosity();
		
		switch (Verbosity)
		{
		case ELogVerbosity::Fatal:
		case ELogVerbosity::Error:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaAdvancedInventoryLoggingVerbosity::Error);
			break;
		case ELogVerbosity::Warning:
		case ELogVerbosity::Verbose:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaAdvancedInventoryLoggingVerbosity::Warning);
			break;
		case ELogVerbosity::Display:
			isVerbosityAllowed = EnumHasAnyFlags(AllowedLogging, EMounteaAdvancedInventoryLoggingVerbosity::Info);
			break;
		case ELogVerbosity::VeryVerbose:
			isVerbosityAllowed = false;
			break;
		case ELogVerbosity::Log:
			isVerbosityAllowed = false;
			break;
		default:
			isVerbosityAllowed = false;
			break;
		}
	}
	
	if (isVerbosityAllowed)
	{
#if WITH_EDITOR
		FMsg::Logf(__FILE__, __LINE__, LogMounteaAdvancedInventorySystem.GetCategoryName(), Verbosity, TEXT("%s"), *Message);
#endif
		
		UKismetSystemLibrary::PrintString(GWorld, Message, true, true, Color, Duration);
	}
}