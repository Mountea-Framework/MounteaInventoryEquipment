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


#include "Statics/MounteaInventoryBaseUIStatics.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"
#include "Widgets/MounteaInventoryWidgetThemeExtension.h"

#include "Components/ActorComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/UserInterface/MounteaAdvancedInventorySharedHUDInterface.h"
#include "Interfaces/Widgets/MounteaInventorySystemWrapperWidgetInterface.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Subsystems/MounteaAdvancedInventorySharedHUDSubsystem.h"


namespace MounteaWidgetTheme
{
	using FClassRules = TMap<TWeakObjectPtr<UClass>, TMap<FName, FMounteaWidgetThemeStyleAssignment>>;
	static TMap<TWeakObjectPtr<const UMounteaAdvancedInventoryGlobalUIConfig>, FClassRules> PrefixCache;

	bool ResolveTextForeground(const UMounteaAdvancedInventoryThemeConfig* Theme, const FMounteaThemeTextStyle* Type,
		const FName OverridePaint, FLinearColor& OutColour)
	{
		if (!OverridePaint.IsNone())
			return Theme->ResolvePaint(OverridePaint, OutColour);

		if (!Type)
			return false;

		if (!Type->Foreground.IsNone())
			return Theme->ResolvePaint(Type->Foreground, OutColour);

		if (Theme->DefaultInk.IsNone())
			return false;

		const FLinearColor* ink = Theme->Palette.Find(Theme->DefaultInk);

		if (!ink)
			return false;

		OutColour = FLinearColor(ink->R, ink->G, ink->B, 1.f);
		return true;
	}

	bool Supports(UClass* WidgetClass, const EMounteaWidgetThemeStyleType StyleType)
	{
		switch (StyleType)
		{
			case EMounteaWidgetThemeStyleType::Text:
				return WidgetClass->IsChildOf(UTextBlock::StaticClass());
			case EMounteaWidgetThemeStyleType::Brush:
				return WidgetClass->IsChildOf(UBorder::StaticClass()) || WidgetClass->IsChildOf(UImage::StaticClass());
			case EMounteaWidgetThemeStyleType::ImageTint:
				return WidgetClass->IsChildOf(UImage::StaticClass());
			case EMounteaWidgetThemeStyleType::Control:
				return WidgetClass->IsChildOf(UButton::StaticClass()) || WidgetClass->IsChildOf(USlider::StaticClass()) ||
					WidgetClass->IsChildOf(UProgressBar::StaticClass()) || WidgetClass->IsChildOf(UEditableTextBox::StaticClass());
			case EMounteaWidgetThemeStyleType::GroupOpacity:
				return WidgetClass->IsChildOf(UWidget::StaticClass());
			default:
				return false;
		}
	}

	const FClassRules& GetRules(const UMounteaAdvancedInventoryGlobalUIConfig* Config)
	{
		if (const FClassRules* cached = PrefixCache.Find(Config))
			return *cached;

		for (auto iterator = PrefixCache.CreateIterator(); iterator; ++iterator)
		{
			if (!iterator.Key().IsValid())
				iterator.RemoveCurrent();
		}

		FClassRules& index = PrefixCache.Add(Config);

		for (const FMounteaWidgetPrefixGroup& group : Config->WidgetPrefixGroups)
		{
			if (!group.WidgetClass)
				continue;

			auto& classRules = index.FindOrAdd(group.WidgetClass.Get());

			for (const FMounteaWidgetPrefixRule& rule : group.Rules)
			{
				if (!Supports(group.WidgetClass.Get(), rule.Style.StyleType))
					continue;

				for (const FName alias : rule.Prefixes)
				{
					const FString prefix = alias.ToString();
					int32 underscore;

					if (!prefix.FindChar(TEXT('_'), underscore) || underscore != prefix.Len() - 1)
						continue;

					if (!classRules.Contains(alias))
						classRules.Add(alias, rule.Style);
				}
			}
		}

		return index;
	}

	void PatchButtonState(const UMounteaAdvancedInventoryThemeConfig* Theme, const FMounteaThemeControlStyle& Control,
		const EMounteaThemeControlState State, FSlateBrush& Brush, FSlateColor& Foreground)
	{
		const FMounteaThemeControlStateStyle* normal = Control.States.Find(EMounteaThemeControlState::Normal);
		const FMounteaThemeControlStateStyle* state = Control.States.Find(State);

		if (normal)
		{
			Theme->PatchBrush(normal->Background, Brush);
			FLinearColor colour;

			if (Theme->ResolvePaint(normal->Foreground, colour))
				Foreground = FSlateColor(colour);
		}

		if (state && state != normal)
		{
			Theme->PatchBrush(state->Background, Brush);
			FLinearColor colour;

			if (Theme->ResolvePaint(state->Foreground, colour))
				Foreground = FSlateColor(colour);
		}
	}

	void Apply(UWidget* Widget, const UMounteaAdvancedInventoryThemeConfig* Theme, const FMounteaWidgetThemeStyleAssignment& Assignment)
	{
		FLinearColor colour;

		if (Assignment.StyleType == EMounteaWidgetThemeStyleType::GroupOpacity)
		{
			if (const float* opacity = Theme->GroupOpacity.Find(Assignment.GroupOpacityRole))
				Widget->SetRenderOpacity(FMath::Clamp(*opacity, 0.f, 1.f));
		}
		else if (Assignment.StyleType == EMounteaWidgetThemeStyleType::Text)
		{
			UTextBlock* text = Cast<UTextBlock>(Widget);
			const FMounteaThemeTextStyle* type = Theme->Typography.Find(Assignment.TypeRole);

			if (!text || !type)
				return;

			text->SetFont(type->Font);

			if (ResolveTextForeground(Theme, type, Assignment.ForegroundPaint, colour))
				text->SetColorAndOpacity(FSlateColor(colour));

			if (Theme->ResolvePaint(type->ShadowColour, colour))
			{
				text->SetShadowColorAndOpacity(colour);
				text->SetShadowOffset(type->ShadowOffset);
			}
		}
		else if (Assignment.StyleType == EMounteaWidgetThemeStyleType::ImageTint)
		{
			UImage* image = Cast<UImage>(Widget);

			if (image && Theme->ResolvePaint(Assignment.TintPaint, colour))
				image->SetColorAndOpacity(colour);
		}
		else if (Assignment.StyleType == EMounteaWidgetThemeStyleType::Brush)
		{
			if (UBorder* border = Cast<UBorder>(Widget))
			{
				FSlateBrush brush = border->Background;
				Theme->PatchBrush(Assignment.BrushRole, brush);
				border->SetBrush(brush);

				if (Theme->ResolvePaint(Assignment.TintPaint, colour))
					border->SetBrushColor(colour);
			}
			else if (UImage* image = Cast<UImage>(Widget))
			{
				FSlateBrush brush = image->GetBrush();
				Theme->PatchBrush(Assignment.BrushRole, brush);
				image->SetBrush(brush);

				if (Theme->ResolvePaint(Assignment.TintPaint, colour))
					image->SetColorAndOpacity(colour);
			}
		}
		else if (Assignment.StyleType == EMounteaWidgetThemeStyleType::Control)
		{
			const FMounteaThemeControlStyle* control = Theme->Controls.Find(Assignment.ControlRole);

			if (!control)
				return;

			if (UButton* button = Cast<UButton>(Widget))
			{
				FButtonStyle style = button->GetStyle();
				PatchButtonState(Theme, *control, EMounteaThemeControlState::Normal, style.Normal, style.NormalForeground);
				PatchButtonState(Theme, *control, EMounteaThemeControlState::Hovered, style.Hovered, style.HoveredForeground);
				PatchButtonState(Theme, *control, EMounteaThemeControlState::Pressed, style.Pressed, style.PressedForeground);
				PatchButtonState(Theme, *control, EMounteaThemeControlState::Disabled, style.Disabled, style.DisabledForeground);
				button->SetStyle(style);
			}
			else if (USlider* slider = Cast<USlider>(Widget))
			{
				FSliderStyle style = slider->GetWidgetStyle();
				Theme->PatchBrush(control->Slider.NormalBar, style.NormalBarImage);
				Theme->PatchBrush(control->Slider.HoveredBar, style.HoveredBarImage);
				Theme->PatchBrush(control->Slider.DisabledBar, style.DisabledBarImage);
				Theme->PatchBrush(control->Slider.NormalThumb, style.NormalThumbImage);
				Theme->PatchBrush(control->Slider.HoveredThumb, style.HoveredThumbImage);
				Theme->PatchBrush(control->Slider.DisabledThumb, style.DisabledThumbImage);
				slider->SetWidgetStyle(style);
			}
			else if (UProgressBar* progress = Cast<UProgressBar>(Widget))
			{
				FProgressBarStyle style = progress->GetWidgetStyle();
				Theme->PatchBrush(control->Progress.Background, style.BackgroundImage);
				Theme->PatchBrush(control->Progress.Fill, style.FillImage);
				Theme->PatchBrush(control->Progress.Marquee, style.MarqueeImage);
				progress->SetWidgetStyle(style);

				if (Theme->ResolvePaint(Assignment.TintPaint, colour))
					progress->SetFillColorAndOpacity(colour);
			}
			else if (UEditableTextBox* input = Cast<UEditableTextBox>(Widget))
			{
				FEditableTextBoxStyle style = input->WidgetStyle;
				const auto& parts = control->EditableText;
				const FMounteaThemeTextStyle* type = Theme->Typography.Find(parts.TypeRole);

				if (type)
					style.SetFont(type->Font);

				if (ResolveTextForeground(Theme, type, parts.ForegroundPaint, colour))
					style.ForegroundColor = FSlateColor(colour);

				Theme->PatchBrush(parts.Background, style.BackgroundImageNormal);
				Theme->PatchBrush(parts.HoveredBackground, style.BackgroundImageHovered);
				Theme->PatchBrush(parts.FocusedBackground, style.BackgroundImageFocused);
				Theme->PatchBrush(parts.ReadOnlyBackground, style.BackgroundImageReadOnly);
				input->WidgetStyle = style;
				input->SynchronizeProperties();
			}
		}
	}
}

void UMounteaInventoryBaseUIStatics::InvalidateWidgetThemePrefixCache(const UMounteaAdvancedInventoryGlobalUIConfig* Config)
{
	MounteaWidgetTheme::PrefixCache.Remove(Config);
}

UMounteaAdvancedInventoryThemeConfig* UMounteaInventoryBaseUIStatics::GetWidgetTheme(UUserWidget* OwnerWidget)
{
	UObject* current = OwnerWidget;

	while (current)
	{
		if (const UUserWidget* widget = Cast<UUserWidget>(current))
		{
			if (const auto* context = widget->GetExtension<UMounteaInventoryWidgetThemeExtension>())
				return context->Theme;
		}

		if (const UWidget* widget = Cast<UWidget>(current))
		{
			if (UPanelWidget* parent = widget->GetParent())
			{
				current = parent;
				continue;
			}
		}

		current = current->GetOuter();
	}

	return nullptr;
}

bool UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(UUserWidget* OwnerWidget, UMounteaAdvancedInventoryThemeConfig* Theme)
{
	if (!IsValid(OwnerWidget) || !IsValid(Theme))
		return false;

	auto* context = OwnerWidget->GetExtension<UMounteaInventoryWidgetThemeExtension>();

	if (context && context->Initialized && context->Theme == Theme && !OwnerWidget->IsDesignTime())
		return true;

	if (!context)
		context = OwnerWidget->AddExtension<UMounteaInventoryWidgetThemeExtension>();

	context->Theme = Theme;
	context->Initialized = false;
	const UMounteaAdvancedInventoryGlobalUIConfig* config = GetGlobalUIConfig();

	if (!config || !OwnerWidget->WidgetTree || !OwnerWidget->WidgetTree->RootWidget)
		return false;
	const auto& index = MounteaWidgetTheme::GetRules(config);
	TArray<UUserWidget*> children;
	OwnerWidget->WidgetTree->ForEachWidget([&](UWidget* widget)
	{
		if (UUserWidget* child = Cast<UUserWidget>(widget))
			children.Add(child);

		const FString name = widget->GetName();
		int32 underscore;

		if (!name.FindChar(TEXT('_'), underscore) || underscore == name.Len() - 1)
			return;

		const FName prefix(*name.Left(underscore + 1));

		for (UClass* widgetClass = widget->GetClass(); widgetClass; widgetClass = widgetClass->GetSuperClass())
		{
			const auto* rules = index.Find(widgetClass);

			if (!rules)
				continue;

			if (const auto* assignment = rules->Find(prefix))
			{
				MounteaWidgetTheme::Apply(widget, Theme, *assignment);
				break;
			}
		}
	});
	context->Initialized = true;

	for (UUserWidget* child : children)
	{
		ApplyWidgetThemeByPrefix(child, Theme);
	}

	return true;
}

APlayerController* UMounteaInventoryBaseUIStatics::FindPlayerController(AActor* Actor, int SearchDepth)
{
	SearchDepth++;
	if (SearchDepth >= 8)
		return nullptr;

	if (!Actor)
		return nullptr;

	if (APlayerController* playerController = Cast<APlayerController>(Actor))
		return playerController;

	if (APawn* actorPawn = Cast<APawn>(Actor))
		return Cast<APlayerController>(actorPawn->GetController());

	if (AActor* ownerActor = Actor->GetOwner())
		return FindPlayerController(ownerActor, SearchDepth);

	return nullptr;
}

bool UMounteaInventoryBaseUIStatics::IsValidBaseUIHandler(const UObject* Target)
{
	return IsValid(Target) && Target->Implements<UMounteaInventorySystemWrapperWidgetInterface>();
}

UMounteaAdvancedInventorySharedHUDSubsystem* UMounteaInventoryBaseUIStatics::GetSharedHUDSubsystem(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	APlayerController* playerController = Cast<APlayerController>(Context);
	if (!IsValid(playerController))
	{
		if (AActor* actor = Cast<AActor>(Context))
			playerController = FindPlayerController(actor, 3);
		else if (const UActorComponent* actorComp = Cast<UActorComponent>(Context))
		{
			if (AActor* ownerActor = actorComp->GetOwner())
				playerController = FindPlayerController(ownerActor, 3);
		}
		else if (const UUserWidget* userWidget = Cast<UUserWidget>(Context))
			playerController = FindPlayerController(userWidget->GetOwningPlayer(), 2);
	}

	if (!IsValid(playerController))
		return nullptr;

	const ULocalPlayer* localPlayer = playerController->GetLocalPlayer();
	return IsValid(localPlayer) ? localPlayer->GetSubsystem<UMounteaAdvancedInventorySharedHUDSubsystem>() : nullptr;
}

UMounteaAdvancedInventoryGlobalUIConfig* UMounteaInventoryBaseUIStatics::GetGlobalUIConfig()
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	return IsValid(settings) ? settings->GlobalUIConfig.LoadSynchronous() : nullptr;
}

bool UMounteaInventoryBaseUIStatics::CreateWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_CreateWrapperWidget(Target.GetObject()) : false;
}

UUserWidget* UMounteaInventoryBaseUIStatics::GetWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetWrapperWidget(Target.GetObject()) : nullptr;
}

void UMounteaInventoryBaseUIStatics::RemoveWrapperWidget(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveWrapperWidget(Target.GetObject());
}

void UMounteaInventoryBaseUIStatics::ExecuteWidgetCommandFromManager(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FString& Command,
	UObject* OptionalPayload)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_ExecuteWidgetCommand(Target.GetObject(), Command, OptionalPayload);
}

UWidget* UMounteaInventoryBaseUIStatics::GetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? Target->Execute_GetNotificationContainer(Target.GetObject()) : nullptr;
}

void UMounteaInventoryBaseUIStatics::SetNotificationContainer(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	UWidget* NewNotificationContainer)
{
	if (Target.GetObject())
		Target->Execute_SetNotificationContainer(Target.GetObject(), NewNotificationContainer);
}

void UMounteaInventoryBaseUIStatics::CreateInventoryNotification(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FInventoryNotificationData& NotificationData)
{
	if (Target.GetObject())
		Target->Execute_CreateInventoryNotification(Target.GetObject(), NotificationData);
}

void UMounteaInventoryBaseUIStatics::RemoveInventoryNotifications(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		Target->Execute_RemoveInventoryNotifications(Target.GetObject());
}

FGameplayTagContainer UMounteaInventoryBaseUIStatics::GetManagerWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_GetWidgetStates(Target.GetObject()) : FGameplayTagContainer();
}

void UMounteaInventoryBaseUIStatics::SetManagerWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTagContainer& NewStates)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_SetWidgetStates(Target.GetObject(), NewStates);
}

bool UMounteaInventoryBaseUIStatics::AddWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_AddWidgetStateTag(Target.GetObject(), Tag) : false;
}

bool UMounteaInventoryBaseUIStatics::RemoveWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_RemoveWidgetStateTag(Target.GetObject(), Tag) : false;
}

bool UMounteaInventoryBaseUIStatics::HasWidgetStateTag(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTag& Tag, const bool bExactMatch)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_HasWidgetStateTag(Target.GetObject(), Tag, bExactMatch) : false;
}

void UMounteaInventoryBaseUIStatics::ClearWidgetStateTags(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	if (Target.GetObject())
		return IMounteaAdvancedInventorySharedHUDInterface::Execute_ClearWidgetStateTags(Target.GetObject());
}

bool UMounteaInventoryBaseUIStatics::AppendWidgetStateTags(
	const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target,
	const FGameplayTagContainer& TagsToAppend)
{
	return Target.GetObject() ?
		IMounteaAdvancedInventorySharedHUDInterface::Execute_AppendWidgetStateTags(Target.GetObject(), TagsToAppend) : false;
}

bool UMounteaInventoryBaseUIStatics::HasAnyWidgetStates(const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Target)
{
	return Target.GetObject() ? !GetManagerWidgetStates(Target).IsEmpty() : false;
}

void UMounteaInventoryBaseUIStatics::InitializeWrapperWidget(UObject* Target, const TScriptInterface<IMounteaAdvancedInventorySharedHUDInterface>& Parent)
{
	if (IsValidBaseUIHandler(Target))
		IMounteaInventorySystemWrapperWidgetInterface::Execute_InitializeWrapperWidget(Target, Parent);
}

void UMounteaInventoryBaseUIStatics::RemoveBaseWrapperWidget(UObject* Target)
{
	if (IsValidBaseUIHandler(Target))
		IMounteaInventorySystemWrapperWidgetInterface::Execute_RemoveWrapperWidget(Target);
}

bool UMounteaInventoryBaseUIStatics::AddModalWindow(UObject* Target, UUserWidget* ModalWindow)
{
	return IsValidBaseUIHandler(Target) ? IMounteaInventorySystemWrapperWidgetInterface::Execute_AddModalWindow(Target, ModalWindow) : false;
}

bool UMounteaInventoryBaseUIStatics::CleanupModalWindow(UObject* Target)
{
	return IsValidBaseUIHandler(Target) ? IMounteaInventorySystemWrapperWidgetInterface::Execute_CleanupModalWindow(Target) : false;
}

TMap<int32, FMounteaTransformedWidgetPositionData> UMounteaInventoryBaseUIStatics::CalculateArcWidgetLayout(
	UCanvasPanel* ParentCanvas, const TMap<int32, UUserWidget*>& CurrentChildren, const FVector2D SizeOverride)
{
	if (!IsValid(ParentCanvas))
		return {};

	const FVector2D canvasSize = SizeOverride.IsZero() ? ParentCanvas->GetCachedGeometry().GetLocalSize() : SizeOverride;
	return CalculateArcWidgetLayoutInternal(canvasSize, CurrentChildren);
}

bool UMounteaInventoryBaseUIStatics::ApplyArcWidgetLayout(
	UCanvasPanel* ParentCanvas, const TMap<int32, UUserWidget*>& CurrentChildren, const FVector2D SizeOverride,
	const bool bSizeToContent)
{
	if (!IsValid(ParentCanvas))
		return false;

	const FVector2D canvasSize = SizeOverride.IsZero() ? ParentCanvas->GetCachedGeometry().GetLocalSize() : SizeOverride;
	const TMap<int32, FMounteaTransformedWidgetPositionData> layout =
		CalculateArcWidgetLayoutInternal(canvasSize, CurrentChildren);

	bool bAppliedAny = false;
	for (const TPair<int32, FMounteaTransformedWidgetPositionData>& pair : layout)
	{
		UUserWidget* widget = pair.Value.Widget;
		if (!IsValid(widget))
			continue;

		UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(widget->Slot);
		if (!canvasSlot)
			continue;

		// Position/rotation are computed as the widget's center on the curve. Force both the slot's
		// anchor and the render pivot to center so they agree — otherwise Position lands the widget's
		// top-left corner on the curve instead, and rotating around the (different) visual center
		// swings the widget off the curve it was just placed on.
		canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		widget->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));

		if (bSizeToContent)
			canvasSlot->SetAutoSize(true);

		canvasSlot->SetPosition(pair.Value.Position);
		widget->SetRenderTransformAngle(pair.Value.RotationAngle);

		bAppliedAny = true;
	}

	return bAppliedAny;
}

TMap<int32, FMounteaTransformedWidgetPositionData> UMounteaInventoryBaseUIStatics::CalculateArcWidgetLayoutInternal(
	const FVector2D& CanvasSize, const TMap<int32, UUserWidget*>& CurrentChildren)
{
	TMap<int32, FMounteaTransformedWidgetPositionData> result;

	if (CurrentChildren.IsEmpty() || CanvasSize.X <= 0.f || CanvasSize.Y <= 0.f)
		return result;

	// Semi-ellipse spanning the whole Canvas: center at bottom-middle, horizontal radius reaching the
	// left/right edges, vertical radius reaching the top edge. Its two ends sit exactly on the Canvas's
	// own bottom-left/bottom-right corners, and its apex sits exactly on the top-center of the Canvas.
	const float radiusX = CanvasSize.X * 0.5f;
	const float radiusY = CanvasSize.Y;
	const FVector2D ellipseCenter(radiusX, CanvasSize.Y);

	TArray<int32> sortedKeys;
	CurrentChildren.GetKeys(sortedKeys);
	sortedKeys.Sort();

	const int32 count = sortedKeys.Num();

	for (int32 rank = 0; rank < count; ++rank)
	{
		UUserWidget* widget = CurrentChildren.FindRef(sortedKeys[rank]);
		if (!IsValid(widget))
			continue;

		// Divide the CANVAS WIDTH (not the arc's angle) into (count + 1) equal linear gaps, and place each
		// child at an interior boundary — so the margin before the first child, the margin after the last
		// child, and the gaps between children are all equal HORIZONTAL distances. A single child lands
		// dead-center. Equal steps in angle would NOT give this: cos()/sin() aren't linear, so equal-angle
		// spacing bunches items toward the apex on a wide/short canvas instead of spreading them evenly.
		const float localX = ((rank + 1) * CanvasSize.X / static_cast<float>(count + 1)) - radiusX;
		const float cosPhi = FMath::Clamp(localX / radiusX, -1.f, 1.f);
		const float phi = FMath::Acos(cosPhi);

		const float localY = -radiusY * FMath::Sin(phi);

		FMounteaTransformedWidgetPositionData data;
		data.Widget = widget;
		data.Position = ellipseCenter + FVector2D(localX, localY);
		// Rotate to match the ellipse's true tangent at this point (the derivative of the position
		// formula above, not the position vector itself) — the two only coincide when radiusX == radiusY,
		// i.e. a circle. Using the position-vector angle here instead visibly misaligns everything
		// between the apex and the two corners on a non-square canvas.
		data.RotationAngle = FMath::RadiansToDegrees(FMath::Atan2(radiusY * FMath::Cos(phi), radiusX * FMath::Sin(phi)));

		result.Add(sortedKeys[rank], data);
	}

	return result;
}
