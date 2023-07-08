#pragma once

#include "Modules/ModuleManager.h"

#include "IAssetTools.h"
#include "Interfaces/IHttpRequest.h"

class FHttpModule;
DECLARE_LOG_CATEGORY_EXTERN(MounteaInventoryEquipmentEditor, All, All);

class FMounteaInventoryEquipmentEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;
	
	void RegisterAssetTypeAction(IAssetTools& AssetTools, const TSharedRef<IAssetTypeActions>& Action);
	
	void OnGetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	UFUNCTION() void SendHTTPGet();

	void PluginButtonClicked();
	void RegisterMenus();

	void RegisterClassIcons(const FString& Path, const FString& ClassName);

private:
	
	TSharedPtr<class FUICommandList> PluginCommands;
	FHttpModule* Http = nullptr;
	
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

	TSharedPtr<FSlateStyleSet> InventoryEquipmentClassStyleSet;
	
	TArray<FName> RegisteredCustomClassLayouts;
	TArray<FName> RegisteredCustomPropertyTypeLayout;
};