
#pragma once

#include "GameFramework/Actor.h"

#include "StageBuildZone.generated.h"

struct FBuildStageTower;

USTRUCT(BlueprintType)
struct MY_API FStageBuildContent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Kind = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageBuildZone : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    int32 GetPosition() const;

    UFUNCTION(BlueprintCallable)
    void Reset();

    UFUNCTION(BlueprintCallable)
    void Select();

    UFUNCTION(BlueprintCallable)
    void Deselect();

    UFUNCTION(BlueprintPure)
    TArray<FBuildStageTower> GetBuildTower();

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnReset();

    UFUNCTION(BlueprintImplementableEvent)
    void OnSelect();

    UFUNCTION(BlueprintImplementableEvent)
    void OnDeselect();

    //UFUNCTION(BlueprintImplementableEvent)
    //void OnBuild(ATowerBase* Tower);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FStageBuildContent> BuildContents;
};
