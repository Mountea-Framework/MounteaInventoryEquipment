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
#include "Delegates/Delegate.h"

enum class EMounteaEditorNotificationType : uint8
{
	Info,
	Success,
	Warning,
	Error
};

/**
 * Shared Editor-only toast notification helper. C++-only (no Blueprint exposure) — usable from any
 * class in this module (Factories, Slate widgets, module startup code, etc.) so the whole Editor
 * module has one canonical way to show the user a result, instead of duplicating
 * FSlateNotificationManager/FNotificationInfo boilerplate at each call site.
 */
struct FMounteaEditorNotificationHelpers
{
	/** Fire-and-forget Editor toast (bottom-right corner). Icon/color driven by Type. */
	static void ShowNotification(
		const FString& Message,
		EMounteaEditorNotificationType Type,
		const FString& SubText = TEXT(""),
		const FText& HyperlinkText = FText::GetEmpty(),
		const FSimpleDelegate& Hyperlink = FSimpleDelegate());
};
