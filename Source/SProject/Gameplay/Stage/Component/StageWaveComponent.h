#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Gameplay/Stage/Types/StageWaveTypes.h"
#include "Gameplay/Stage/Types/StageMonsterGroupTypes.h"

#include "Table/MonsterGroupTable.h"

#include "StageWaveComponent.generated.h"

class AStageLevel;
struct FStageWaveGroupInfo;
struct FStageMonsterGroupInfo;
struct FMonsterGroupTableRow;

UCLASS(Abstract, Blueprintable, meta = (BlueprintSpawnableComponent), Category = "Stage", ClassGroup = "Stage")
class MY_API UStageWaveComponent : public UGGameCoreComponent
{
	GENERATED_BODY()

public:
	// UActorComponent
	virtual void InitializeComponent() override;
	// ~UActorComponent

public:
	UFUNCTION(BlueprintCallable)
	FGErrorInfo Setup(int32 InStageLevel, int32 InWave);

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "웨이브가 진행 중인지 판별합니다."))
	bool IsPlaying() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "모든 웨이브를 클리어 했는지 판별합니다."))
	bool IsComplete() const;

	UFUNCTION(BlueprintPure, meta = (ShortToolTip = "현재 웨이브를 반환합니다."))
	int32 GetWave() const;

public:
	UPROPERTY(BlueprintReadOnly)
	int32 Wave = INDEX_NONE;

private:
	int32 WaveGroup = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TArray<FStageWaveGroupInfo> WaveGroupInfo;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveStart();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWaveEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStageWaveComplete();

public:
	UPROPERTY(BlueprintReadWrite)
	bool Paused = true;

	UFUNCTION(BlueprintCallable)
	FGErrorInfo SetWaveGroup(int32 WaveGroup);

	UFUNCTION(BlueprintCallable)
	FGErrorInfo WaveStart();

	UFUNCTION(BlueprintCallable)
	FGErrorInfo WaveEnd();

	UFUNCTION(BlueprintCallable)
	FGErrorInfo NextWave();

	UFUNCTION(BlueprintCallable)
	TArray<FStageWaveGroupInfo> GetWaveGroupInfos();

	UFUNCTION(BlueprintCallable)
	TArray<FMonsterGroupTableRow> GetCurrentMonsterGroupInfo();

	UFUNCTION(BlueprintCallable)
	TArray<FMonsterGroupTableRow> GetMonsterGroupInfos(int32 MonsterGroup);

};