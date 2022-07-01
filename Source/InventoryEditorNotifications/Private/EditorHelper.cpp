// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorHelper.h"

#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"


#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#endif

void FEditorHelper::DisplayWarningEditorNotification(FText DisplayText, SNotificationItem::ECompletionState State, float ExpireDuration, float FadeOutDuration)
{
#if WITH_EDITOR
	if (DisplayText.IsEmpty() || State == SNotificationItem::CS_None) return;
	
	
	FNotificationInfo NotificationInfo = FNotificationInfo(DisplayText);
	
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.ExpireDuration = ExpireDuration;
	NotificationInfo.FadeOutDuration = FadeOutDuration;
	
	FSlateNotificationManager::Get().AddNotification(NotificationInfo).Get()->SetCompletionState(State);
#endif
}

void FEditorHelper::BFS(const TSharedRef<SWidget> Parent)
{
	TArray<TSharedRef<SWidget>> Queue;
	Queue.Add(Parent);

	while (Queue.Num() > 0)
	{
		const TSharedRef<SWidget> Widget = Queue[0];
		Queue.RemoveAt(0);

		FChildren* Children = Widget->GetAllChildren();
		if (Children == nullptr) continue;
		int32 ChildNum = Children->Num();

		for (int32 i = 0; i < Children->Num(); i++)
		{
			Queue.Add(Children->GetChildAt(i));
		}
	}
}
