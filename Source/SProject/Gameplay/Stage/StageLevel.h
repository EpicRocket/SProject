
#pragma once

#include "Gameplay/GameplayLevel.h"

#include "StageLevel.generated.h"

class AGameplayPathActor;
class AStageBuildZone;
class AStagePlayerPawn;
class AStageSpawner;
class AStageStartPoint;
class AStageSupervisor;
struct FStage;

UCLASS()
class MY_API AStageLevel : public AGameplayLevel
{
	GENERATED_BODY()

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// IGLoadingProcess
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcess
	
	FGErrorInfo Setup(int32 InStageLevel, TSubclassOf<AStageSupervisor> InSupervisorClass);

	UFUNCTION(BlueprintCallable)
	void AddBuildZone(AStageBuildZone* BuildZone);

	UFUNCTION(BlueprintPure)
	AStageBuildZone* GetBuildZone(int32 Position) const;

	UFUNCTION(BlueprintPure)
	TArray<AStageBuildZone*> GetBuildZones() const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerPawn(AStagePlayerPawn* InPlayerPawn);

	UFUNCTION(BlueprintPure)
	APawn* GetPlayerPawn() const;

	UFUNCTION(BlueprintCallable)
	void AddPathActor(AGameplayPathActor* PathActor);

	UFUNCTION(BlueprintPure)
	AGameplayPathActor* GetPathActor(int32 Position) const;

	UFUNCTION(BlueprintPure)
	TArray<AGameplayPathActor*> GetPathActors() const;

	UFUNCTION(BlueprintCallable)
	void AddSpawner(AStageSpawner* Spawner);

	UFUNCTION(BlueprintPure)
	AStageSpawner* GetSpawner(int32 Position) const;

	UFUNCTION(BlueprintPure)
	TArray<AStageSpawner*> GetSpawners() const;

	UFUNCTION(BlueprintCallable)
	void AddStartPoint(AStageStartPoint* StartPoint);

	UFUNCTION(BlueprintPure)
	AStageStartPoint* GetStartPoint(int32 Position) const;

	UFUNCTION(BlueprintPure)
	TArray<AStageStartPoint*> GetStartPoints() const;

protected:
	virtual void OnInitailize();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	AStageSupervisor* Supervisor;

	TMap<int32, TWeakObjectPtr<AGameplayPathActor>> PathActors;

	TMap<int32, TWeakObjectPtr<AStageBuildZone>> BuildZones;

	TMap<int32, TWeakObjectPtr<AStageSpawner>> Spawners;

	TMap<int32, TWeakObjectPtr<AStageStartPoint>> StartPoints;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStagePlayerPawn> PlayerPawn;

	UPROPERTY(Transient, BlueprintReadOnly)
	int32 StageLevel = INDEX_NONE;

};
