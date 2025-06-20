// Copyright (c) 2025 Team EpicRocket. All rights reserved.

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
			new []
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
				"AIModule",
				"ModularGameplay",
			}
		);

		// GameplayAbility
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
			}
		);

		// GameFeatures
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"GameFeatures",
			}
		);

		// UI
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"CommonUI",
				"CommonInput",
			}
		);

		// Loading
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"PreLoadScreen",
			}
		);

		// 2D
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Paper2D",
			}
		);
	}
}
