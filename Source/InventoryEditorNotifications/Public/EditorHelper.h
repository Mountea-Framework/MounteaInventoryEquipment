// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Notifications/SNotificationList.h"

#define PATH_TO_ICON_WARNING TEXT("/Resources/WarningIcon_128")

struct INVENTORYEDITORNOTIFICATIONS_API FEditorHelper
{

public:
	
	/** Displays editor notification. */
	static void DisplayWarningEditorNotification(FText DisplayText, SNotificationItem::ECompletionState State, float ExpireDuration = 5.0f, float FadeOutDuration = 2.0f);
	
	/** Level Order Traversal (BFS (Breadth-First Search)) for Slate Widgets. */
	static void BFS(TSharedRef<SWidget> Parent);

	
};
