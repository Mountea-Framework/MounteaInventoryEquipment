// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "MounteaBaseUserWidget.generated.h"

class UMounteaInventoryThemeConfig;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FMounteaDynamicDelegate, const FString&, Command, UObject*, Payload) ;

USTRUCT(BlueprintType)
struct FMounteaDynamicDelegateContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Command;

	UPROPERTY(BlueprintReadWrite)
	UObject* Payload = nullptr;
};

USTRUCT(BlueprintType)
struct FMounteaEventBinding
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	const UObject* Listener;

	UPROPERTY(VisibleAnywhere)
	FString CallbackFunction;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag BindingTag;

	UPROPERTY(VisibleAnywhere)
	FName BindingName;

	UPROPERTY(VisibleAnywhere)
	FMounteaDynamicDelegate Delegate;

	bool operator==(const FMounteaDynamicDelegate& InDelegate) const
	{
		return Delegate == InDelegate;
	}

	bool operator==(const FMounteaEventBinding& Other) const
	{
		return

		Other.Delegate.IsBound() ?

		Delegate == Other.Delegate

		:
		
		Other.BindingName.IsValid() ?
			
		Listener == Other.Listener &&
		CallbackFunction == Other.CallbackFunction &&
		BindingTag == Other.BindingTag &&
		BindingName == Other.BindingName
		
		:
		
		Listener == Other.Listener &&
		CallbackFunction == Other.CallbackFunction &&
		BindingTag == Other.BindingTag;
	}
};

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

	UMounteaBaseUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	UPROPERTY(EditAnywhere, Category="1. Optional", meta=(AllowAbstract=false, DisplayThumbnail=false))
	UMounteaInventoryThemeConfig* ThemeConfigOverride = nullptr;

public:
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Bind Delegate", NativeMakeFunc, Keywords="listen,to,set,make"))
	bool BindDelegate(const FMounteaDynamicDelegate& Delegate, const FGameplayTag BindingTag, const FName OptionalName);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Unbind Delegate", NativeMakeFunc, Keywords="remove,clear,listen"))
	bool UnbindDelegate(const FMounteaDynamicDelegate& Delegate, const FGameplayTag& BindingTag, const FName OptionalName);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Call Event", NativeMakeFunc, Keywords="call,broadcast,fire"))
	bool CallEvent(const FGameplayTag& EventTag, const FName OptionalName, const FMounteaDynamicDelegateContext& Context);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Remove Bindings (Filtered)", NativeMakeFunc, Keywords="clean,unbind,restore"))
	bool RemoveBindings(const FGameplayTag& EventTag, const FName OptionalName);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Remove All Bindings", NativeMakeFunc, Keywords="clean,unbind,restore"))
	void RemoveAllBindings();

	//UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Listen By Function", NativeMakeFunc))
	//bool StartListeningByFunction(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, FName OptionalName);
	//UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(DisplayName="Stop Listen By Function", NativeMakeFunc))
	//bool StopListeningByFunction(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, FName OptionalName);
	
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Mountea|Debug", meta=(DevelopmentOnly))
	void RenderPreview();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool BindEvents();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")
	bool UnBindEvents();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryThemeConfig"), meta=(DeterminesOutputType = "ClassFilter"))//, meta=(CompactNodeTitle="Theme Config", HideSelfPin=true))
	UMounteaInventoryThemeConfig* GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|Inventory")//, meta=(CompactNodeTitle="Defaults", HideSelfPin=true))
	void LoadFromConfig();

private:

	UPROPERTY(VisibleAnywhere, Category="0. Debug")
	TArray<FMounteaEventBinding> MounteaEventBindings;
};
