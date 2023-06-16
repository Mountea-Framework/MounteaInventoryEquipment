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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UObject* Listener;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CallbackFunction;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag BindingTag;

	UPROPERTY(BlueprintReadWrite)
	FName BindingName;

	UPROPERTY(VisibleAnywhere)
	FMounteaDynamicDelegate Delegate;

	bool operator==(const FMounteaEventBinding& Other) const
	{
		return
		BindingName.IsValid() ?
			
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

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(CompactNodeTitle="ListenEvent"))
	bool StartListeningToEvent(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, FName OptionalName);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(CompactNodeTitle="StopListenEvent"))
	bool StopListeningToEvent(UObject* Listener, const FString& CallbackFunction, const FGameplayTag& BindingTag, FName OptionalName);

	UFUNCTION(BlueprintCallable, Category="Mountea|Inventory", meta=(CompactNodeTitle="CallEvent"))
	bool CallEvent(const FGameplayTag& EventTag, FName OptionalName, const FMounteaDynamicDelegateContext& Context);

	
	
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
