// All rights reserved Dominik Morse 2024


#pragma once

// All rights reserved Dominik Pavlicek 2023

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
