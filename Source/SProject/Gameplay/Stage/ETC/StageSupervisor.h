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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserStatusChanged, int32, OldValue, int32, NewValue);

UCLASS(Abstract, Blueprintable, BlueprintType, Category = "Stage", ClassGroup = "Stage", hidecategories = (Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Transformation, Physics))
class MY_API AStageSupervisor : public AInfo
{
	GENERATED_BODY()

	// Actor
protected:
	virtual void BeginPlay() override;
	// ~Actor

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn")
	class UStageSpawnComponent* GetSpawnComponent();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FGErrorInfo SpawnTower(uint8 TeamID, FVector Location, FRotator Rotation, FStageTowerInfo TowerInfo, AStageTowerUnit*& SpawnedUnit);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FGErrorInfo SpawnMonster(uint8 TeamID, FVector Location, FRotator Rotation, FStageMonsterInfo MonsterInfo, AStageMonsterUnit*& SpawnedUnit);

	/*UFUNCTION(BlueprintCallable, Category = "Spawn", meta = (DeterminesOutputType = "StageUnitClass", DynamicOutputParam = "SpawnedUnit"))
	FGErrorInfo K2_SpawnUnit(uint8 TeamID, FVector Location, FRotator Rotation, TSubclassOf<AStageUnitCharacter> StageUnitClass, AStageUnitCharacter*& SpawnedUnit);

	template<typename T>
	FGErrorInfo SpawnUnit(uint8 TeamID, FVector Location, FRotator Rotation, UClass* Class, T*& SpawnedUnit)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const AStageUnitCharacter>::Value, TEXT("템플릿 'T'는 AStageUnitCharacter를 상속 받아야 합니다."));
		AStageUnitCharacter* Temp = nullptr;
		auto Err = K2_SpawnUnit(TeamID, Location, Rotation, TSubclassOf<AStageUnitCharacter>{Class}, Temp);
		SpawnedUnit = (T*)Temp;
		return Err;
	}*/

	UFUNCTION(BlueprintCallable)
	void SetHp(int32 NewValue);

	UFUNCTION(BlueprintPure)
	int32 GetHp() const;

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int32 NewValue);

	UFUNCTION(BlueprintPure)
	int32 GetUsePoint() const;

	UFUNCTION(BlueprintPure)
	FGErrorInfo PayUsePoint(int32 Cost);

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
