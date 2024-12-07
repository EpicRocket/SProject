
#pragma once

#include "Gameplay/MyGameLevel.h"

#include "StageLevel.generated.h"

class AStageBuildZone;
class AStagePlayerPawn;

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

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnInitailize();

public:
    TMap<int32, TWeakObjectPtr<AStageBuildZone>> BuildZones;

    UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStagePlayerPawn> PlayerPawn;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 Level = INDEX_NONE;
};
