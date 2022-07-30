// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FActorInventoryEditorUtilities
{
public:

	static bool PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class);
};