// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FMIEPopupStyle
{
public:

	static void Create();
	static void Initialize();
	static void Shutdown();
	static ISlateStyle& Get()
	{
		return *(StyleSet.Get());
	}

	static const FSlateBrush * GetBrush(FName PropertyName, const ANSICHAR* Specifier = NULL)
	{
		return StyleSet->GetBrush(PropertyName, Specifier);
	};
	
	static const FName& GetStyleSetName()
	{
		static FName StyleSetName(TEXT("MIEPopupStyleSet"));
		return StyleSetName;
	};

	template< class T >            
	static const T& GetWidgetStyle( FName PropertyName, const ANSICHAR* Specifier = NULL  ) 
	{
		return StyleSet->GetWidgetStyle< T >( PropertyName, Specifier );
	}

private:
	static TSharedPtr<FSlateStyleSet> StyleSet;
};
