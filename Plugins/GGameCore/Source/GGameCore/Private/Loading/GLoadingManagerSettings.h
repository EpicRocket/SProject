
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/SoftObjectPath.h"

#include "GLoadingManagerSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class GGAMECORE_API UGLoadingManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGLoadingManagerSettings();

public:
	UPROPERTY(Config, EditDefaultsOnly, meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;

	UPROPERTY(Config, EditDefaultsOnly)
	int32 LoadingScreenZOrder = 10000;

	UPROPERTY(Config, EditDefaultsOnly, meta = (ForceUnits = s, ConsoleVariable = "GLoading.HoldLoadingScreenAdditionalSecs"))
	float HoldLoadingScreenAdditionalSecs = 2.0F;

	UPROPERTY(Config, EditDefaultsOnly, meta = (ForceUnits = s))
	float LoadingScreenHeartbeatHangDuration = 0.0F;

	UPROPERTY(Config, EditDefaultsOnly, meta = (ForceUnits = s))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	UPROPERTY(config, EditDefaultsOnly)
	bool ForceTickLoadingScreenEvenInEditor = true;

	UPROPERTY(Transient, EditDefaultsOnly, meta = (ConsoleVariable = "GLoading.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = 0;

	UPROPERTY(Transient, EditDefaultsOnly, meta = (ConsoleVariable = "GLoading.AlwaysShow"))
	bool ForceLoadingScreenVisible = false;

	UPROPERTY(Transient, EditDefaultsOnly)
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;
};
