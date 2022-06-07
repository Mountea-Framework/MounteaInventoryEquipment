// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Log category definition
ACTORINVENTORYPLUGIN_API DECLARE_LOG_CATEGORY_EXTERN(LogActorInventory, Display, All);

#define AIP_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogActorInvenotry, Verbosity, Format, ##__VA_ARGS__); \
}