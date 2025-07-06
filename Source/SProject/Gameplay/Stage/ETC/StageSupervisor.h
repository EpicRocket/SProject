// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "StageSupervisor.generated.h"

struct FStage;

UCLASS(Blueprintable, BlueprintType, Category = "Stage", ClassGroup = "Stage")
class MY_API AStageSupervisor : public AInfo
{
	GENERATED_BODY()

	// Actor
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~Actor

public:


private:
	UFUNCTION()
	void OnTableLoaded();

	void OnGameplayDataLoad();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> OwnerLevel;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStateComponent> StageStateComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStorageComponent> StageStorageComponent;

private:
	UPROPERTY(Transient)
	class UStageTableReceipt* StageTableReceipt = nullptr;

	TWeakPtr<FStage> Stage;
};
