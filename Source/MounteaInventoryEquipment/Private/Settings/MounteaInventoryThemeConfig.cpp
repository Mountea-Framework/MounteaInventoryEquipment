// All rights reserved Dominik Pavlicek 2023.


#include "Settings/MounteaInventoryThemeConfig.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"


UMounteaInventoryThemeConfig::UMounteaInventoryThemeConfig()
{
}

void UMounteaInventoryThemeConfig::GenerateMissingThemes()
{
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
		const TSet<UMounteaInventoryItemCategory*> AllCategories = UMounteaInventoryEquipmentBPF::GetAllowedCategories();
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

#endif