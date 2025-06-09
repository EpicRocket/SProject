// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Core/Component/GGameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/IGLoadingProcess.h"

#include "StageStateComponent.generated.h"

struct FLatentActionInfo;
struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UStageStateComponent : public UGGameStateComponent, public IGLoadingProcess
{
	GENERATED_BODY()

public:
	// UActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	// ~UActorComponent

	// IGLoadingProcess
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcess

	UFUNCTION(BlueprintCallable)
	FGErrorInfo LoadStage(const FStage& Stage);

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo WaitForPrimaryPlayerController(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStage(const FStage& Stage, const TSoftObjectPtr<class UWorld>& Level);

private:
	UFUNCTION()
	void OnTableLoading();

	UFUNCTION()
	void OnTableLoaded();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> TargetStage;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bLoadCompleted = false;
};
