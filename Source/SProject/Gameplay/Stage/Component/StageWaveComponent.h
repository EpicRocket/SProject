#pragma once

#include "Core/Component/GGameCoreComponent.h"

#include "Table/MonsterGroupTable.h"

#include "StageWaveComponent.generated.h"

class AStageLevel;
class UStageMonsterContext;
struct FStage;

USTRUCT(BlueprintType)
struct MY_API FStageSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UniqueId = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Wave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double AmountDelayTime = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Spawner = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Path = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double SpawnDelay = 0.0;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UStageMonsterContext> MonsterContext;

};

USTRUCT(BlueprintType)
struct MY_API FStageSpawnParam
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UStageMonsterContext> Context = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Spawner = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Path = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnCount = 0;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FRequestStageSpawnEvent, FStageSpawnParam, Param);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageWaveEvent, int32, Wave);

UCLASS(Abstract, Blueprintable, meta = (BlueprintSpawnableComponent), Category = "Stage", ClassGroup = "Stage")
class MY_API UStageWaveComponent : public UGGameCoreComponent
{
	GENERATED_BODY()

public:
	UStageWaveComponent(const FObjectInitializer& ObjectInitializer);

	// UActorComponent
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~UActorComponent

public:
	FGErrorInfo Setup(TSharedPtr<FStage> Stage);

	FGErrorInfo Start();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetStage", ReturnDisplayName = "Error"))
	FGErrorInfo K2_GetStage(UPARAM(DisplayName = "ReturnValue") FStage& CurrentStage) const;
	TSharedPtr<FStage> GetStage() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "다음 웨이브 시작이 잠겼는지 판별합니다."))
	bool IsNextWaveLocked() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "웨이브가 진행 중인지 판별합니다."))
	bool IsPlaying() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "모든 웨이브를 클리어 했는지 판별합니다."))
	bool IsComplete() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "현재 웨이브를 반환합니다."))
	int32 GetWave() const;

protected:
	void OnSpawn(FStageSpawnParam Param);

public:
	UPROPERTY(EditAnywhere, Category = "Events", meta = (IsBindableEvent = true))
	FRequestStageSpawnEvent RequestSpawnEvent;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Events")
	FStageWaveEvent WaveStartEvent;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Events")
	FStageWaveEvent WaveEndedEvent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 LastWave = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 NextWave = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	double NextWaveDelay = 0.0;

private:
	UPROPERTY(Transient)
	int32 SequenceId = 0;

	TWeakPtr<FStage> StagePtr;

	UPROPERTY(Transient)
	TSet<int32> PlayedWaves;

	UPROPERTY(Transient)
	TArray<FStageSpawnData> SpawnDatas;

};