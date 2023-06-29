// Fill out your copyright notice in the Description page of Project Settings.


#include "SMappableConfigPair.h"

#include "CommonUISettings.h"
#include "ICommonUIModule.h"
#include "SAssetManager.h"
#include "SGameUserSettings.h"
#include "PlayerMappableInputConfig.h"

bool FMappableConfigPair::CanBeActivated() const
{
	const FGameplayTagContainer& PlatformTraits = ICommonUIModule::GetSettings().GetPlatformTraits();

	// If the current platform does NOT have all the dependent traits, then don't activate it
	if (!DependentPlatformTraits.IsEmpty() && !PlatformTraits.HasAll(DependentPlatformTraits))
	{
		return false;
	}

	// If the platform has any of the excluded traits, then we shouldn't activate this config.
	if (!ExcludedPlatformTraits.IsEmpty() && PlatformTraits.HasAny(ExcludedPlatformTraits))
	{
		return false;
	}

	return true;
}

bool FMappableConfigPair::RegisterPair(const FMappableConfigPair& Pair)
{
	USAssetManager& AssetManager = USAssetManager::Get();

	if (USGameUserSettings* Settings = USGameUserSettings::Get())
	{
		// Register the pair with the settings, but do not activate it yet
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.PlayerMappableInputConfig))
		{
			Settings->RegisterInputConfig(Pair.Type, LoadedConfig, false);
			return true;
		}	
	}
	
	return false;
}

void FMappableConfigPair::UnregisterPair(const FMappableConfigPair& Pair)
{
	USAssetManager& AssetManager = USAssetManager::Get();

	if (USGameUserSettings* Settings = USGameUserSettings::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.PlayerMappableInputConfig))
		{
			Settings->UnregisterInputConfig(LoadedConfig);
		}
	}
}

bool FMappableConfigPair::ActivatePair(const FMappableConfigPair& Pair)
{
	USAssetManager& AssetManager = USAssetManager::Get();

	if (USGameUserSettings* Settings = USGameUserSettings::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.PlayerMappableInputConfig))
		{
			Settings->ActivateInputConfig(LoadedConfig);
			return true;
		}
	}
	return false;
}

void FMappableConfigPair::DeactivatePair(const FMappableConfigPair& Pair)
{
	USAssetManager& AssetManager = USAssetManager::Get();

	if (USGameUserSettings* Settings = USGameUserSettings::Get())
	{
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Pair.PlayerMappableInputConfig))
		{
			Settings->DeactivateInputConfig(LoadedConfig);
		}
	}
}
