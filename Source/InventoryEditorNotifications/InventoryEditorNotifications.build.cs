using UnrealBuildTool;
 
public class InventoryEditorNotifications : ModuleRules
{
	public InventoryEditorNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd", "Slate", "SlateCore", "Projects"});
 
		PublicIncludePaths.AddRange(new string[] {"InventoryEditorNotifications/Public"});
		PrivateIncludePaths.AddRange(new string[] {"InventoryEditorNotifications/Private"});
	}
}