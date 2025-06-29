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

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMounteaAdvancedInventoryLoggingVerbosity : uint8
{
	None		= 0			UMETA(hidden),
	// Toggle Info On/Off. Info level provides most basic information. Color is green.
	Info		= 1 << 0,
	// Toggle Warning On/Off. Yellow color. Warning level provides information about issues that might affect Inventory functionality, but are not blockers.
	Warning		= 1 << 1,
	// Toggle Error On/Off. Red color. Error level provides information about issues that will block Inventory functionality.
	Error		= 1 << 2
};
ENUM_CLASS_FLAGS(EMounteaAdvancedInventoryLoggingVerbosity)
