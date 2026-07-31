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

#include "MounteaEditorNotificationHelpers.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Styling/MounteaAdvancedInventoryEditorStyle.h"

void FMounteaEditorNotificationHelpers::ShowNotification(
	const FString& Message,
	EMounteaEditorNotificationType Type,
	const FString& SubText,
	const FText& HyperlinkText,
	const FSimpleDelegate& Hyperlink)
{
	FNotificationInfo notifInfo(FText::FromString(Message));
	notifInfo.bFireAndForget = true;
	notifInfo.ExpireDuration = 4.0f;
	notifInfo.bUseThrobber = false;
	notifInfo.bUseLargeFont = false;
	notifInfo.WidthOverride = FOptionalSize();

	if (!SubText.IsEmpty())
		notifInfo.SubText = FText::FromString(SubText);

	if (Hyperlink.IsBound() && !HyperlinkText.IsEmpty())
	{
		notifInfo.Hyperlink = Hyperlink;
		notifInfo.HyperlinkText = HyperlinkText;
	}

	SNotificationItem::ECompletionState completionState = SNotificationItem::CS_None;
	switch (Type)
	{
	case EMounteaEditorNotificationType::Success:
		notifInfo.Image = FMounteaAdvancedInventoryEditorStyle::GetBrush(TEXT("MAISStyleSet.Success"));
		completionState = SNotificationItem::CS_Success;
		break;
	case EMounteaEditorNotificationType::Warning:
		notifInfo.Image = FMounteaAdvancedInventoryEditorStyle::GetBrush(TEXT("MAISStyleSet.Warning"));
		completionState = SNotificationItem::CS_Fail;
		break;
	case EMounteaEditorNotificationType::Error:
		notifInfo.Image = FMounteaAdvancedInventoryEditorStyle::GetBrush(TEXT("MAISStyleSet.Error"));
		completionState = SNotificationItem::CS_Fail;
		break;
	case EMounteaEditorNotificationType::Info:
	default:
		break;
	}

	const TSharedPtr<SNotificationItem> notificationItem = FSlateNotificationManager::Get().AddNotification(notifInfo);
	if (notificationItem.IsValid() && completionState != SNotificationItem::CS_None)
		notificationItem->SetCompletionState(completionState);
}
