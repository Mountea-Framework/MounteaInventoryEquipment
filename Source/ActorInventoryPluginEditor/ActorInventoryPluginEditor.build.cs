using UnrealBuildTool;
 
public class ActorInventoryPluginEditor : ModuleRules
{
	public ActorInventoryPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		
		PrecompileForTargets = PrecompileTargetsType.None;
		bPrecompile = false;
		bUsePrecompiled = false;

        PublicDependencyModuleNames.AddRange
        (
	        new string[]
	        {
		        "Core",
		        "CoreUObject",
					
		        "BlueprintGraph",
					
		        "Engine",
					
		        "KismetCompiler",
		        "PropertyEditor",
		        "UnrealEd",
		        "Kismet"
	        }
        );
		
        PrivateDependencyModuleNames.AddRange
        (
	        new string[]
	        {
		        "ActorInventoryPlugin",
		        "UnrealEd",
		        "Projects",
				
		        "Slate",
		        "SlateCore",
				
		        "AssetTools",
				
		        "BlueprintGraph",
		        "Kismet", 
				
		        "WebBrowser",
		        "EditorStyle",
		        "DeveloperSettings",
				
		        "MainFrame",
		        "ToolMenus",
		        "InputCore",
				
		        "UMG"
	        }
        );
	}
}