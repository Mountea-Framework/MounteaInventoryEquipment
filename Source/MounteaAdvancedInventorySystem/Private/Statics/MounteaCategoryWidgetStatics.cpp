// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Statics/MounteaCategoryWidgetStatics.h"

#include "Interfaces/Widgets/Category/MounteaCategoryWidgetEventRegistrarInterface.h"

UObject* UMounteaCategoryWidgetStatics::ResolveRegistrar(const UObject* CategoryWidget)
{
	if (!IsValid(CategoryWidget) || !CategoryWidget->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		return nullptr;

	UObject* registrar = IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_GetCategoryWidgetEventRegistrar(CategoryWidget);
	return (IsValid(registrar) && registrar->Implements<UMounteaCategoryWidgetEventRegistrarInterface>()) ? registrar : nullptr;
}

void UMounteaCategoryWidgetStatics::SetCategoryWidgetEventRegistrar(UObject* CategoryWidget, UObject* Registrar)
{
	if (IsValid(CategoryWidget) && CategoryWidget->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SetCategoryWidgetEventRegistrar(CategoryWidget, Registrar);
}

UObject* UMounteaCategoryWidgetStatics::GetCategoryWidgetEventRegistrar(UObject* CategoryWidget)
{
	if (!IsValid(CategoryWidget) || !CategoryWidget->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>())
		return nullptr;

	return IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_GetCategoryWidgetEventRegistrar(CategoryWidget);
}

bool UMounteaCategoryWidgetStatics::BindToOnCategoryWidgetSelected(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding)
{
	UObject* registrar = ResolveRegistrar(CategoryWidget);
	return registrar ? IMounteaCategoryWidgetEventRegistrarInterface::Execute_BindCategoryWidgetSelected(registrar, CategoryWidget, Binding) : false;
}

bool UMounteaCategoryWidgetStatics::UnbindFromOnCategoryWidgetSelected(UObject* CategoryWidget, const FMounteaCategoryWidgetSelectedBinding& Binding)
{
	UObject* registrar = ResolveRegistrar(CategoryWidget);
	return registrar ? IMounteaCategoryWidgetEventRegistrarInterface::Execute_UnbindCategoryWidgetSelected(registrar, CategoryWidget, Binding) : false;
}

bool UMounteaCategoryWidgetStatics::TriggerOnCategoryWidgetSelected(UObject* CategoryWidget)
{
	UObject* registrar = ResolveRegistrar(CategoryWidget);
	if (!registrar)
		return false;

	const FString categoryId = IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_GetInventoryCategoryKey(CategoryWidget);
	return IMounteaCategoryWidgetEventRegistrarInterface::Execute_TriggerCategoryWidgetSelected(registrar, CategoryWidget, categoryId);
}

bool UMounteaCategoryWidgetStatics::SelectCategory(UObject* CategoryWidget)
{
	return IsValid(CategoryWidget) && 
		CategoryWidget->Implements<UMounteaAdvancedInventoryCategoryWidgetInterface>() && 
			IMounteaAdvancedInventoryCategoryWidgetInterface::Execute_SelectCategory(CategoryWidget);
}

FString UMounteaCategoryWidgetStatics::CalculateNewSelectedCategory(const int32 Offset, const TArray<FString>& Categories,
	const FString& ActiveCategory, const bool Overscroll)
{
	const int32 count = Categories.Num();
	const int32 activeIndex = Categories.Find(ActiveCategory);
	if (activeIndex == INDEX_NONE)
		return ActiveCategory;

	const int32 targetIndex = activeIndex + Offset;
	if (targetIndex >= count)
		return Overscroll ? Categories[0] : ActiveCategory;
	if (targetIndex < 0)
		return Overscroll ? Categories[count - 1] : ActiveCategory;

	return Categories[targetIndex];
}
