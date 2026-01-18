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

class SPinTypeCommandSelector : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPinTypeCommandSelector) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
    
private:
	void OnCommandSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo);
	FText GetSelectedCommandText() const;
	TSharedRef<SWidget> GenerateCommandWidget(TSharedPtr<FString> Item);
    
	TArray<TSharedPtr<FString>> CommandOptions;
};

#endif