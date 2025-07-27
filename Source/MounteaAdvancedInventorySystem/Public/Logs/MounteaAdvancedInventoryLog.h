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

// Log category declaration
MOUNTEAADVANCEDINVENTORYSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogMounteaAdvancedInventorySystem, Log, All);

MOUNTEAADVANCEDINVENTORYSYSTEM_API void PrintLog(const ELogVerbosity::Type Verbosity, const FString& Message, FLinearColor Color, float Duration);

// Logging macro definitions
#define LOG_INFO(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Log, FormattedMessage, FLinearColor(0.0f, 1.0f, 0.0f), 5.0f); \
}

#define LOG_WARNING(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Warning, FormattedMessage, FLinearColor(1.0f, 1.0f, 0.0f), 10.0f); \
}

#define LOG_ERROR(Format, ...) \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
PrintLog(ELogVerbosity::Error, FormattedMessage, FLinearColor(1.0f, 0.0f, 0.0f), 15.0f); \
}