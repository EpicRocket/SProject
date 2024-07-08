

using UnrealBuildTool;
using System.IO;

public class SProject : ModuleRules
{
    public SProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "SProject.h";

        PublicIncludePaths.AddRange(
            new[] {
                ModuleDirectory,
            }
        );

        PublicDependencyModuleNames.AddRange(
            new[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "SlateCore",
                "UMG",
                "AIModule",
                "GGameCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[] {
                "Slate",
                "UnrealEd",
                "EnhancedInput",
                "DataValidation",
                "DeveloperSettings",
                "AssetRegistry",
            }
        );

        // Common UI
        PrivateDependencyModuleNames.AddRange(
            new[] {
                "CommonUI",
                "CommonInput",
            }
        );

        // GameplayAbility
        PrivateDependencyModuleNames.AddRange(
            new[] {
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
            }
        );

        // Plugins
        PrivateDependencyModuleNames.AddRange(
            new[] {
                "GameplayMessageRuntime",
            }
        );

        // Generate compile errors if using DrawDebug functions in test/shipping builds.
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");

        SetupGameplayDebuggerSupport(Target);

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
