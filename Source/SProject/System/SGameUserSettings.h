// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "CommonInputBaseTypes.h"
#include "SGameUserSettings.generated.h"

struct FLoadedMappableConfigPair;
class UPlayerMappableInputConfig;
/**
 * 
 */
UCLASS()
class SPROJECT_API USGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	USGameUserSettings();

	static USGameUserSettings* Get();
	
public:
	DECLARE_EVENT_OneParam(USGameUserSettings, FInputConfigDelegate, const FLoadedMappableConfigPair& /*Config*/);

	/** Delegate called when a new input config has been registered */
	FInputConfigDelegate OnInputConfigRegistered;

	/** Delegate called when a registered input config has been activated */
	FInputConfigDelegate OnInputConfigActivated;
	
	/** Delegate called when a registered input config has been deactivate */
	FInputConfigDelegate OnInputConfigDeactivated;
	
	/** Register the given input config with the settings to make it available to the player. */
	void RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive);
	
	/** Unregister the given input config. Returns the number of configs removed. */
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove);

	void ActivateInputConfig(const UPlayerMappableInputConfig* Config);

	void DeactivateInputConfig(const UPlayerMappableInputConfig* Config);

	/** Get all currently registered input configs */
	const TArray<FLoadedMappableConfigPair>& GetAllRegisteredInputConfigs() const { return RegisteredInputConfigs; }

private:
	/**
	 * Array of currently registered input configs. This is populated by game feature plugins
	 * 
	 * @see UGameFeatureAction_AddInputConfig
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FLoadedMappableConfigPair> RegisteredInputConfigs;
};
