
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
	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading", meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading")
	int32 LoadingScreenZOrder = 10000;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading", meta = (ForceUnits = s, ConsoleVariable = "GLoading.HoldLoadingScreenAdditionalSecs"))
	float HoldLoadingScreenAdditionalSecs = 2.0F;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading", meta = (ForceUnits = s))
	float LoadingScreenHeartbeatHangDuration = 0.0F;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading", meta = (ForceUnits = s))
	float LogLoadingScreenHeartbeatInterval = 5.0f;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Loading")
	bool ForceTickLoadingScreenEvenInEditor = true;

	UPROPERTY(EditDefaultsOnly, Category = "Loading", meta = (ConsoleVariable = "GLoading.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Loading", meta = (ConsoleVariable = "GLoading.AlwaysShow"))
	bool ForceLoadingScreenVisible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Transition", meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath TransitionScreenWidget;

};
