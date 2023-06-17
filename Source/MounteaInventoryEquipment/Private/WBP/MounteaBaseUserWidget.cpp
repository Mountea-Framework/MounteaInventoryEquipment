// All rights reserved Dominik Pavlicek 2022.


#include "WBP/MounteaBaseUserWidget.h"

#include "Helpers/MounteaInventoryEquipmentBPF.h"


UMounteaBaseUserWidget::UMounteaBaseUserWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	bIsFocusable = true;
}

void UMounteaBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();

	LoadFromConfig();
}

void UMounteaBaseUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UnBindEvents();
}

/*
bool UMounteaBaseUserWidget::StartListeningByFunction(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, FName OptionalName)
{
	if (Listener == nullptr)
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("StartListeningByFunction requires Listener!"));
		return false;
	}
	
	FName const FunctionFName(*CallbackFunction);

	if (Listener)
	{
		const UFunction* const Func = Listener->FindFunction(FunctionFName);
		if (Func == nullptr)
		{
			UE_LOG(LogBlueprintUserMessages, Error, TEXT("StartListeningByFunction requires valid Function in the Listener!"));
			return false;
		}
		
		if ( Func && (Func->NumParms != 2) )
		{
			// User passed in a valid function, but one that takes parameters
			// FMounteaDynamicDelegate expects 2 parameters and will choke on execution if it tries
			// to execute a mismatched function
			UE_LOG(LogBlueprintUserMessages, Error, TEXT("StartListeningByFunction passed a function (%s) that expects invalid parameters amount."), *CallbackFunction);
			return false;
		}
	}

	FMounteaEventBinding NewBinding;
	NewBinding.Listener = Listener;
	NewBinding.CallbackFunction = CallbackFunction;
	NewBinding.BindingName = OptionalName;
	NewBinding.BindingTag = BindingTag;

	if (MounteaEventBindings.Contains(NewBinding))
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("StartListeningByFunction tried to duplicate (%s) binding!"), *CallbackFunction);
		return false;
	}

	FMounteaDynamicDelegate Delegate;
	Delegate.BindUFunction(Listener, FunctionFName);
	NewBinding.Delegate = Delegate;
	
	MounteaEventBindings.Add(NewBinding);

	return true;
}
*/

bool UMounteaBaseUserWidget::BindDelegate(const FMounteaDynamicDelegate& Delegate, const FGameplayTag BindingTag, const FName OptionalName)
{
	if (!BindingTag.IsValid() && !OptionalName.IsValid())
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("BindDelegate requires valid Gameplay Tag!"));
		return false;
	}
	
	FMounteaEventBinding PossibleBinding;
	PossibleBinding.Delegate = Delegate;
	PossibleBinding.BindingTag = BindingTag;
	PossibleBinding.BindingName = OptionalName;
	PossibleBinding.CallbackFunction = Delegate.GetFunctionName().ToString();
	PossibleBinding.Listener = Delegate.GetUObject();

	if (MounteaEventBindings.Contains(PossibleBinding))
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("BindDelegate tried to duplicate (%s) binding!"), *Delegate.GetFunctionName().ToString());
		return false;
	}

	MounteaEventBindings.Add(PossibleBinding);

	return true;
}

bool UMounteaBaseUserWidget::UnbindDelegate(const FMounteaDynamicDelegate& Delegate, const FGameplayTag& BindingTag, const FName OptionalName)
{
	FMounteaEventBinding DirtyBinding;
	
	for (const auto& Itr : MounteaEventBindings)
	{
		if (Itr.Delegate == Delegate)
		{
			if (Itr.BindingTag == BindingTag)
			{
				if (OptionalName.IsValid())
				{
					if (Itr.BindingName == OptionalName)
					{
						DirtyBinding = Itr;
						break;
					}

					continue;
				}
				
				DirtyBinding = Itr;
				break;
			}
		}
	}

	if (DirtyBinding.Listener == nullptr)
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("UnbindDelegate found no binding for Event (%s) with Tag (%s)!"), *Delegate.GetFunctionName().ToString(), *BindingTag.ToString());
		return false;
	}

	MounteaEventBindings.Remove(DirtyBinding);
	return true;
}

bool UMounteaBaseUserWidget::CallEvent(const FGameplayTag& EventTag, const FName OptionalName, const FMounteaDynamicDelegateContext& Context)
{
	bool bFound = false;
	TArray<FMounteaEventBinding> FoundBindings;
	
	for (const auto& Itr : MounteaEventBindings)
	{
		if (EventTag == Itr.BindingTag)
		{
			if (OptionalName.IsValid())
			{
				if (Itr.BindingName == OptionalName)
				{
					bFound = true;
					FoundBindings.Add(Itr);
				}

				continue;
			}
			
			bFound = true;
			FoundBindings.Add(Itr);
		}
	}

	if (!bFound)
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("CallEvent didn't find any function by Tag (%s) and Name (%s)!"), *OptionalName.ToString(), *EventTag.ToString());
		return false;
	}

	for (const auto& [Listener, CallbackFunction, BindingTag, BindingName, Delegate] : FoundBindings)
	{
		Delegate.ExecuteIfBound(Context.Command, Context.Payload);
	}

	return true;
}

bool UMounteaBaseUserWidget::RemoveAllBindings(const FGameplayTag& EventTag, const FName OptionalName)
{
	TArray<FMounteaEventBinding> DirtyBindings;
	for (const auto& Itr : MounteaEventBindings)
	{
		if (Itr.BindingTag == EventTag)
		{
			if (OptionalName.IsValid())
			{
				if (Itr.BindingName == OptionalName)
				{
					DirtyBindings.Add(Itr);
				}

				continue;
			}

			DirtyBindings.Add(Itr);
		}
	}

	if (DirtyBindings.Num() == 0)
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("RemoveAllBindings didn't find any Bindings by Tag (%s) and Name (%s)!"), *OptionalName.ToString(), *EventTag.ToString());
		return false;
	}

	for (const auto& Itr : DirtyBindings)
	{
		MounteaEventBindings.Remove(Itr);
	}

	return true;
}

/*
bool UMounteaBaseUserWidget::StopListeningByFunction(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, const FName OptionalName)
{
	FMounteaEventBinding PossibleBinding;
	PossibleBinding.Listener = Listener;
	PossibleBinding.CallbackFunction = CallbackFunction;
	PossibleBinding.BindingTag = BindingTag;
	PossibleBinding.BindingName = OptionalName;
	
	if (!MounteaEventBindings.Contains(PossibleBinding))
	{
		UE_LOG(LogBlueprintUserMessages, Error, TEXT("StopListeningByFunction didn't find any existing binding with specified paremeters!"));
		return false;
	}

	MounteaEventBindings.Remove(PossibleBinding);
	
	return true;
}
*/

void UMounteaBaseUserWidget::RenderPreview()
{
	OnWidgetRebuilt();
}

UMounteaInventoryThemeConfig* UMounteaBaseUserWidget::GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult) const
{
	if (ClassFilter == nullptr)
	{
		bResult = false;
		return nullptr;
	}

	bResult = true;
	const auto FoundTheme = ThemeConfigOverride != nullptr ? ThemeConfigOverride : UMounteaInventoryEquipmentBPF::GetThemeConfig(ClassFilter, bResult);

	if (!bResult)
	{
		return NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);
	}

	return FoundTheme->IsA(ClassFilter) ? FoundTheme : NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);
}