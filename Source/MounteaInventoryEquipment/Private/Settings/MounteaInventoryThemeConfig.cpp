// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaInventoryThemeConfig.h"
#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"

UMounteaInventoryThemeConfig::UMounteaInventoryThemeConfig()
{
}

void UMounteaInventoryThemeConfig::GenerateMissingThemes()
{
	// Inventory Image Theme
	{
		if (InventoryTheme == nullptr)
		{
			UInventoryTheme* NewInventoryTheme = NewObject<UInventoryTheme>(GetPackage());
			
			InventoryTheme = NewInventoryTheme;
		}
	}
	
	// Category Image Theme
	{
		if (CategoryImageTheme == nullptr)
		{
			UImageTheme* NewImageTheme = NewObject<UImageTheme>(GetPackage());
			
			CategoryImageTheme = NewImageTheme;
		}
	}
	
	// Categories Themes
	{
		const TSet<UMounteaInventoryItemCategory*> AllCategories = UMounteaInventoryEquipmentBFL::GetAllowedCategories();
		TSet<UMounteaInventoryItemCategory*> CategoriesWithTheme;
	
		for (const auto& Itr : CategoryThemes)
		{
			if (Itr.CategoryTheme) 
			{
				CategoriesWithTheme.Add(Itr.CategoryTheme->Category);
			}
		}

		TArray<UMounteaInventoryItemCategory*> MissingCategories;
		for (UMounteaInventoryItemCategory* Category : AllCategories)
		{
			if (!CategoriesWithTheme.Contains(Category))
			{
				MissingCategories.Add(Category);
			}
		}

		for (const auto& Itr : MissingCategories)
		{
			UCategoryTheme* NewCategoryTheme = NewObject<UCategoryTheme>(GetPackage(), UCategoryTheme::StaticClass());

			if (NewCategoryTheme == nullptr) continue;
			
			NewCategoryTheme->Category = Itr;
			NewCategoryTheme->CategoryStyle = CategoryImageTheme;

			FCategoryThemeData NewCategoryThemeData;
			NewCategoryThemeData.CategoryTheme = NewCategoryTheme;
		
			CategoryThemes.Add(NewCategoryThemeData);
		}
	}

	// Text Theme
	{
		if (TextTheme == nullptr)
		{
			UTextTheme* NewTextTheme = NewObject<UTextTheme>(GetPackage());

			TextTheme = NewTextTheme;
		}
	}

	// Colours Theme
	{
		if (ColoursTheme == nullptr)
		{
			UColoursTheme* NewColoursTheme = NewObject<UColoursTheme>(GetPackage());

			ColoursTheme = NewColoursTheme;
		}
	}
}



#if WITH_EDITOR

void UMounteaInventoryThemeConfig::GenerateMissingThemes_Editor()
{
	GenerateMissingThemes();
}

void UMounteaInventoryThemeConfig::Propagate_CategoryDefaultStyle()
{
	for (const auto& Itr : CategoryThemes)
	{
		if (Itr.CategoryTheme) 
		{
			Itr.CategoryTheme->CategoryStyle = CategoryImageTheme;
		}
	}
}

void UMounteaInventoryThemeConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FString PropertyName = PropertyChangedEvent.Property->GetName();

	// TODO: Rather than making them in-package, create a new ones in the same folder!
	if (PropertyName.Equals("InventoryTheme"))
	{
		GenerateMissingThemes();
	}
	if (PropertyName.Equals("CategoryThemes"))
	{
		GenerateMissingThemes();
	}

	if (PropertyName.Equals("TextTheme"))
	{
		GenerateMissingThemes();
	}

	if (PropertyName.Equals("ColoursTheme"))
	{
		GenerateMissingThemes();
	}
}

bool UMounteaInventoryThemeConfig::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
{
	const bool bSatisfied = Super::Rename(NewName, NewOuter, Flags);

	if (bSatisfied)
	{
		if (InventoryTheme) InventoryTheme->Rename(nullptr, NewOuter, Flags);
		
		if (TextTheme) TextTheme->Rename(nullptr, NewOuter, Flags);

		if (ColoursTheme) ColoursTheme->Rename(nullptr, NewOuter, Flags);

		if (CategoryImageTheme) CategoryImageTheme->Rename(nullptr, NewOuter, Flags);

		for (auto& Itr : CategoryThemes)
		{
			if (Itr.CategoryTheme) Itr.CategoryTheme->Rename(nullptr, NewOuter, Flags);
		}
	}
	
	return bSatisfied;
}

#endif
