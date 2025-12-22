// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MounteaAdvancedInventorySystem : ModuleRules
{
	public MounteaAdvancedInventorySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"GameplayTags", 
				"CommonUI", 
				"GameplayAbilities",
				"GameplayTasks"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				
				"DeveloperSettings",
				"GameplayTags",
				
				"Projects",
				"UMG",
				
				"NetCore",
				
				"InputCore",
				"EnhancedInput",
				
				"Json"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
