﻿
#pragma once

#include "GameFramework/Actor.h"
#include "Engine/DataAsset.h"

#include "StageBuildZone.generated.h"

struct FBuildStageTower;
enum class EStageTowerType : uint8;

USTRUCT(BlueprintType)
struct MY_API FStageBuildContent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStageTowerType TowerType = static_cast<EStageTowerType>(0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Kind = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;
};

UCLASS(BlueprintType)
class MY_API UStageBuildZoneData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FStageBuildContent> BuildContents;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageBuildZone : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    int32 GetPosition() const;

    UFUNCTION(BlueprintCallable)
    TArray<FBuildStageTower> GetBuildTower() const;

    UFUNCTION(BlueprintCallable)
    void Reset();

    UFUNCTION(BlueprintCallable)
    void Select();

    UFUNCTION(BlueprintCallable)
    void Deselect();

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnReset();

    UFUNCTION(BlueprintImplementableEvent)
    void OnSelect();

    UFUNCTION(BlueprintImplementableEvent)
    void OnDeselect();

    //UFUNCTION(BlueprintImplementableEvent)
    //void OnBuild(ATowerBase* Tower);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStageBuildZoneData> BuildZoneData;
};
