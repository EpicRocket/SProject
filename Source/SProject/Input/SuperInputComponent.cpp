// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SuperInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "System/SUPERGameUserSettings.h"
#include "System/SUPERMappableConfigPair.h"
#include "SuperInputConfig.h"

void USuperInputComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USuperInputComponent::AddInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Add any registered input mappings from the settings!
	if (const USuperGameUserSettings* LocalSettings = USuperGameUserSettings::Get())
	{
		// We don't want to ignore keys that were "Down" when we add the mapping context
		// This allows you to die holding a movement key, keep holding while waiting for respawn,
		// and have it be applied after you respawn immediately. Leaving bIgnoreAllPressedKeysUntilRelease
		// to it's default "true" state would require the player to release the movement key,
		// and press it again when they respawn
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;
		
		// Add the input mappings
		const TArray<FLoadedMappableConfigPair> InputConfigs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : InputConfigs)
		{
			if (Pair.bIsActive)
			{
				InputSubsystem->AddPlayerMappableConfig(Pair.Config, Options);
			}
		}
		
		// Tell enhanced input about any custom keymappings that the player may have customized
		// for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		// {
		// 	if (Pair.Key != NAME_None && Pair.Value.IsValid())
		// 	{
		// 		InputSubsystem->AddPlayerMappedKeyInSlot(Pair.Key, Pair.Value);
		// 	}
		// }
	}
}

void USuperInputComponent::RemoveInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
	
	if (const USuperGameUserSettings* LocalSettings = USuperGameUserSettings::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& InputConfigs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : InputConfigs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// // Clear any player mapped keys from enhanced input
		// for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		// {
		// 	InputSubsystem->RemovePlayerMappedKeyInSlot(Pair.Key);
		// }
	}
}
