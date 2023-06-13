// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SProject : ModuleRules
{
	public SProject(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UMG",
				"GameFeatures",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Slate",
				"SlateCore",
				"UnrealEd",
				"EnhancedInput",
				"UIExtension",
				"CommonUI",
				"CommonInput",
				"CommonGame",
				"ModularGameplayActors",
			}
		);

		PublicIncludePaths.AddRange(
			new[]
			{
				ModuleDirectory,
			}
		);

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
