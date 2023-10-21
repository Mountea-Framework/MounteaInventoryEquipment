// All rights reserved Dominik Pavlicek 2023.


#include "Settings/MounteaInventoryThemeConfig.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"


UMounteaInventoryThemeConfig::UMounteaInventoryThemeConfig()
{
	GenerateMissingThemes();
}

void UMounteaInventoryThemeConfig::GenerateMissingThemes()
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
		UCategoryTheme* NewCategoryTheme = NewObject<UCategoryTheme>(GetPackage());
		NewCategoryTheme->Category = Itr;

		FCategoryThemeData NewCategoryThemeData;
		NewCategoryThemeData.CategoryTheme = NewCategoryTheme;
		
		CategoryThemes.Add(NewCategoryThemeData);
	}
}

#if WITH_EDITOR

void UMounteaInventoryThemeConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FString PropertyName = PropertyChangedEvent.Property->GetName();

	if (PropertyName.Equals("CategoryThemes"))
	{
		GenerateMissingThemes();
	}
}

#endif