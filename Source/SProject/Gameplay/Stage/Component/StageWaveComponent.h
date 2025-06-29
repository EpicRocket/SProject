#pragma once

#include "Core/Component/GGameStateComponent.h"
#include "Gameplay/Stage/Types/StageWaveTypes.h"
#include "Gameplay/Stage/Types/StageMonsterGroupTypes.h"

#include "Table/MonsterGroupTable.h"

#include "StageWaveComponent.generated.h"

class AStageLevel;
struct FStageWaveGroupInfo;
struct FStageMonsterGroupInfo;
struct FMonsterGroupTableRow;

UCLASS()
class MY_API UStageWaveComponent : public UGGameStateComponent
{
	GENERATED_BODY()

public:
	// UActorComponent
	virtual void InitializeComponent() override;
	// ~UActorComponent

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