// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "StageSupervisor.generated.h"

struct FStage;
struct FStageTowerInfo;
struct FStageMonsterInfo;
class AStageTowerUnit;
class AStageMonsterUnit;
class AStageUnitCharacter;
class UStageMonsterContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserStatusChanged, int32, OldValue, int32, NewValue);

UCLASS(Abstract, Blueprintable, BlueprintType, Category = "Stage", ClassGroup = "Stage", hidecategories = (Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Transformation, Physics))
class MY_API AStageSupervisor : public AInfo
{
	GENERATED_BODY()

	// Actor
public:
	virtual void PreInitializeComponents() override;
protected:
	virtual void BeginPlay() override;
	// ~Actor

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetStage", ReturnDisplayName = "Error"))
	FGErrorInfo K2_GetStage(UPARAM(DisplayName = "ReturnValue") FStage& CurrentStage) const;
	TSharedPtr<FStage> GetStage() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn")
	class UStageSpawnComponent* GetSpawnComponent();

	UFUNCTION(BlueprintCallable, Category = "Spawn", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo RegisterSpawnedUnit(AStageUnitCharacter* Unit);

	UFUNCTION(BlueprintCallable, Category = "Spawn", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo UnregisterSpawnedUnit(AStageUnitCharacter* Unit);

	UFUNCTION(BlueprintCallable, Category = "Spawn", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo SpawnTower(uint8 InTeamID, FVector InLocation, FRotator InRotation, FStageTowerInfo InTowerInfo, AStageTowerUnit*& SpawnedTower);

	UFUNCTION(BlueprintCallable, Category = "Spawn", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo SpawnMonster(uint8 InTeamID, FVector InLocation, FRotator InRotation, FStageMonsterInfo InMonsterInfo, AStageMonsterUnit*& SpawnedMonster);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	class UStageWaveComponent* GetWaveComponent();

	UFUNCTION(BlueprintCallable, Category = "Wave", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo RequestStartWave();

	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetHp(int32 NewValue);

	UFUNCTION(BlueprintPure, Category = "Status")
	int32 GetHp() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetUsePoint(int32 NewValue);

	UFUNCTION(BlueprintPure, Category = "Status")
	int32 GetUsePoint() const;

	UFUNCTION(BlueprintPure, Category = "Status", meta = (ReturnDisplayName = "Error"))
	FGErrorInfo PayUsePoint(int32 Cost);

protected:
	UFUNCTION(BlueprintCallable, meta = (ReturnDisplayName = "Error"))
	FGErrorInfo StartStage();

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

	UFUNCTION(BlueprintCallable, meta = (ReturnDisplayName = "Error"))
	FGErrorInfo ResetStageData();

private:
	UFUNCTION()
	void OnWaveMonsterSpawn(UStageMonsterContext* Context, int32 Position, int32 SpawnCount);

private:
	FGErrorInfo OnTableLoaded();
	FGErrorInfo OnGameplayDataLoad();
	FGErrorInfo OnGameplayDataLoad_Wave();
	FGErrorInfo OnGameplayDataLoad_User();
	FGErrorInfo OnGameplayDataReload();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> OwnerLevel;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStateComponent> StageStateComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UStageStorageComponent> StageStorageComponent;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FOnUserStatusChanged OnHpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Status")
	FOnUserStatusChanged OnUsePointChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<class AStageAIController> AIControllerClass;

private:
	UPROPERTY(Transient)
	class UStageTableReceipt* StageTableReceipt = nullptr;

	TWeakPtr<FStage> StagePtr;

	UPROPERTY(Transient)
	TMap<FString, TWeakObjectPtr<AStageUnitCharacter>> SpawnedUnits;

};
