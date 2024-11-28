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
                "InputCore",
                "RenderCore",
                "DeveloperSettings",
            }
		);

		// GameplayAbility
		PublicDependencyModuleNames.AddRange(
			new[] {
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
			}
		);

		// GameplayMessage
		PublicDependencyModuleNames.AddRange(
			new[] {
                "GameplayMessageRuntime",
			}
		);

		// UI
		PublicDependencyModuleNames.AddRange(
			new[] {
				"CommonUI",
				"CommonInput",
			}
		);

		// Loading
		PublicDependencyModuleNames.AddRange(
			new[] {
				"PreLoadScreen",
			}
		);

        // 2D
        PublicDependencyModuleNames.AddRange(
            new[] {
                "Paper2D",
            }
        );
    }
}
