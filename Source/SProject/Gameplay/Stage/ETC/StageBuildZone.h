﻿
#pragma once

#include "GameFramework/Actor.h"

#include "StageBuildZone.generated.h"

class ATowerBase;

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
    
    UFUNCTION(BlueprintCallable)
    void Build(TSubclassOf<ATowerBase> TowerClass);

    UFUNCTION(BlueprintCallable)
    ATowerBase* GetSpawendTower() const;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnReset();

    UFUNCTION(BlueprintImplementableEvent)
    void OnSelect();

    UFUNCTION(BlueprintImplementableEvent)
    void OnDeselect();

    UFUNCTION(BlueprintImplementableEvent)
    void OnBuild(ATowerBase* Tower);

private:
    UPROPERTY(Transient)
    TObjectPtr<ATowerBase> SpawendTower;
};