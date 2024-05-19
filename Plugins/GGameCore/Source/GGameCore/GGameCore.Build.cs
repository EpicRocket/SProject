// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GGameCore : ModuleRules
{
	public GGameCore(ReadOnlyTargetRules Target) : base(Target)
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
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "Core",
                "CommonInput",
                "CommonUI",
                "GameplayTags",
            }
		);
	}
}
