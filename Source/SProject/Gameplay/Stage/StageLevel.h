
#pragma once

#include "Gameplay/MyGameLevel.h"

#include "StageLevel.generated.h"

class AGameplayPathActor;
class AStageBuildZone;
class AStagePlayerPawn;
class AStageSpawner;

UCLASS()
class MY_API AStageLevel : public AMyGameLevel
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

	UFUNCTION(BlueprintCallable)
	void AddBuildZone(AStageBuildZone* BuildZone);

	UFUNCTION(BlueprintCallable)
	AStageBuildZone* GetBuildZone(int32 Position) const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerPawn(AStagePlayerPawn* InPlayerPawn);

	UFUNCTION(BlueprintCallable)
	void AddPathActor(AGameplayPathActor* PathActor);

	UFUNCTION(BlueprintCallable)
	AGameplayPathActor* GetPathActor(int32 Position) const;

	UFUNCTION(BlueprintCallable)
	void AddSpawner(AStageSpawner* Spawner);

	UFUNCTION(BlueprintCallable)
	AStageSpawner* GetSpawner(int32 Position) const;

protected:
	virtual void OnInitailize();

public:
	TMap<int32, TWeakObjectPtr<AGameplayPathActor>> PathActors;

	TMap<int32, TWeakObjectPtr<AStageBuildZone>> BuildZones;

	TMap<int32, TWeakObjectPtr<AStageSpawner>> Spawners;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStagePlayerPawn> PlayerPawn;

};
