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
				"Engine",
				"JsonUtilities",
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
				"Slate",
				"SlateCore",
				
				"DeveloperSettings",
				"GameplayTags",
				
				"Projects",
				"UMG",
				
				"NetCore",
				
				"InputCore",
				"EnhancedInput",
				
				"Json",
				"CommonInput",
			}
			);
		
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("AssetRegistry");
		}


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
