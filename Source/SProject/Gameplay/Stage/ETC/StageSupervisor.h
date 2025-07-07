// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "StageSupervisor.generated.h"

struct FStage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserStatusChanged, int32, OldValue, int32, NewValue);

UCLASS(Abstract, Blueprintable, BlueprintType, Category = "Stage", ClassGroup = "Stage", hidecategories = (Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Transformation, Physics))
class MY_API AStageSupervisor : public AInfo
{
	GENERATED_BODY()

	// Actor
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~Actor

public:
	UFUNCTION(BlueprintImplementableEvent)
	class UStageSpawnComponent* GetSpawnComponent();

	UFUNCTION(BlueprintCallable)
	void SetHp(int32 NewValue);

	UFUNCTION(BlueprintPure)
	int32 GetHp() const;

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int32 NewValue);

	UFUNCTION(BlueprintPure)
	int32 GetUsePoint() const;

protected:
	UFUNCTION(BlueprintCallable)
	void StartStage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewStage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayingStage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDefeatStage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnClearStage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCompletedStage();

	UFUNCTION(BlueprintCallable)
	void ResetStageData();

private:
	UFUNCTION()
	void OnTableLoaded();

	void OnGameplayDataLoad();

	void OnGameplayDataReload();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> OwnerLevel;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStateComponent> StageStateComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStorageComponent> StageStorageComponent;

	UPROPERTY(BlueprintAssignable)
	FOnUserStatusChanged OnHpChanged;

	UPROPERTY(BlueprintAssignable)
	FOnUserStatusChanged OnUsePointChanged;

private:
	UPROPERTY(Transient)
	class UStageTableReceipt* StageTableReceipt = nullptr;

	TWeakPtr<FStage> Stage;

};
