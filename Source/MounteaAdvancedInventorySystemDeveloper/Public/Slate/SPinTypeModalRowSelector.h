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

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Widgets/Input/SComboBox.h"

class UMounteaAdvancedInventoryGlobalUIConfig;

class SPinTypeModalRowSelector : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPinTypeModalRowSelector) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	virtual EVisibility GetDefaultValueVisibility() const override;

private:
	void RefreshModalRowOptions();
	void OnModalRowSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo);
	FText GetSelectedModalRowText() const;
	TSharedRef<SWidget> GenerateModalRowWidget(TSharedPtr<FString> Item);
	void BuildModalRowOptions(TArray<FString>& OutModalRows) const;
	FString ResolveModalType() const;
	UMounteaAdvancedInventoryGlobalUIConfig* ResolveGlobalUIConfig() const;

	TArray<TSharedPtr<FString>> ModalRowOptions;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ModalRowComboBox;
};

#endif
