// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Settings/MounteaInventoryThemeConfig.h"
#if WITH_EDITOR
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#endif

#include "AssetToolsModule.h"
#include "Factories/DataAssetFactory.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"

UMounteaInventoryThemeConfig::UMounteaInventoryThemeConfig()
{
}

UInventoryTheme* UMounteaInventoryThemeConfig::GetInventoryTheme() const
{
	if (InventoryTheme) return InventoryTheme;
	return NewObject<UInventoryTheme>(GetPackage());
}

void UMounteaInventoryThemeConfig::GenerateMissingThemes()
{

#if WITH_EDITOR
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	const FString FilePath = GetFilePath(this);
	
	if (InventoryTheme == nullptr)
	{
		UInventoryTheme* NewInventoryTheme = CreateNewAsset<UInventoryTheme>(FilePath);
		if (NewInventoryTheme)
		{
			InventoryTheme = NewInventoryTheme;
		}
	}

	// Category Image Theme
	{
		if (CategoryImageTheme == nullptr)
		{
			UImageTheme* NewImageTheme = CreateNewAsset<UImageTheme>(FilePath);
			
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
			UCategoryTheme* NewCategoryTheme = CreateNewAsset<UCategoryTheme>(FilePath);

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
			UTextTheme* NewTextTheme = CreateNewAsset<UTextTheme>(FilePath);

			TextTheme = NewTextTheme;
		}
	}

	// Colours Theme
	{
		if (ColoursTheme == nullptr)
		{
			UColoursTheme* NewColoursTheme = CreateNewAsset<UColoursTheme>(FilePath);

			ColoursTheme = NewColoursTheme;
		}
	}

#else

	// Inventory Image Theme
	{
		if (InventoryTheme == nullptr)
		{
			UInventoryTheme* NewInventoryTheme = NewObject<UInventoryTheme>(GetOuter()->GetPackage());
			
			InventoryTheme = NewInventoryTheme;
		}
	}
	
	// Category Image Theme
	{
		if (CategoryImageTheme == nullptr)
		{
			UImageTheme* NewImageTheme = NewObject<UImageTheme>(GetOuter()->GetPackage());
			
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
			UCategoryTheme* NewCategoryTheme = NewObject<UCategoryTheme>(GetOuter()->GetPackage(), UCategoryTheme::StaticClass());

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
			UTextTheme* NewTextTheme = NewObject<UTextTheme>(GetOuter()->GetPackage());

			TextTheme = NewTextTheme;
		}
	}

	// Colours Theme
	{
		if (ColoursTheme == nullptr)
		{
			UColoursTheme* NewColoursTheme = NewObject<UColoursTheme>(GetOuter()->GetPackage());

			ColoursTheme = NewColoursTheme;
		}
	}
	
#endif
	
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
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, InventoryTheme))
	{
		GenerateMissingThemes();
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, TextTheme))
	{
		GenerateMissingThemes();
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, ColoursTheme))
	{
		GenerateMissingThemes();
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, CategoryImageTheme))
	{
		GenerateMissingThemes();
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, CategoryThemes))
	{
		GenerateMissingThemes();
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaInventoryThemeConfig, ButtonStyleThemes))
	{
		GenerateMissingThemes();
	}
}

bool UMounteaInventoryThemeConfig::Rename(const TCHAR* NewName, UObject* NewOuter, ERenameFlags Flags)
{
	const bool bSatisfied = Super::Rename(NewName, NewOuter, Flags);
	
	if (bSatisfied)
	{
		FString PropertyName;
		
		if (InventoryTheme)
		{
			InventoryTheme->Rename(nullptr, NewOuter, Flags);
		}
		
		if (TextTheme)
		{
			TextTheme->Rename(nullptr, NewOuter, Flags);
		}

		if (ColoursTheme)
		{
			ColoursTheme->Rename(nullptr, NewOuter, Flags);
		}

		if (CategoryImageTheme)
		{
			CategoryImageTheme->Rename(nullptr, NewOuter, Flags);
		}

		for (auto& Itr : CategoryThemes)
		{
			if (Itr.CategoryTheme)
			{
				Itr.CategoryTheme->Rename(nullptr, NewOuter, Flags);
			}
		}
	}
	
	return bSatisfied;
}

#endif
