﻿
#pragma once

#include "Components/GameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

class UStageDataAsset;
class AStageLevel;
class AMyGameLevel;
class UWorld;
class APlayerController;
struct FGErrorInfo;
struct FLatentActionInfo;

UCLASS()
class MY_API UStageStateComponent : public UGameStateComponent, public IGLoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	
	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

public:
	UFUNCTION(BlueprintCallable)
	FGErrorInfo SetLevel(int32 Level);

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo OnLoadStage(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadCompleted();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStreaming();

	UFUNCTION()
	void OnLoadLevelCompleted();
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnLoadLevelCompleted"))
	void K2_OnLoadLevelCompleted();
	
	UFUNCTION(BlueprintCallable)
	void SetTargetLevel(AMyGameLevel* Level);

public:
	UPROPERTY(EditDefaultsOnly, Category = "에셋")
	TSoftObjectPtr<UStageDataAsset> StageDataAsset;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStageLevel> TargetStage;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LoadLevel;

	bool bLoadCompleted = false;
};
