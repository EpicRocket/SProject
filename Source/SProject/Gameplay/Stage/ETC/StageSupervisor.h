// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"

#include "StageSupervisor.generated.h"

class AStageLevel;
//class UStageTableReceipt;

UCLASS(Blueprintable, BlueprintType, Category = "Stage", ClassGroup = "Stage")
class MY_API AStageSupervisor : public AInfo
{
    GENERATED_BODY()

    // Actor
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~Actor

private:
    /*UFUNCTION()
    void OnTableLoaded(UStageTableReceipt* Receipt);*/

public:
    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<AStageLevel> OwnerLevel;

private:
    /*UPROPERTY()
	UStageTableReceipt* StageTableReceipt = nullptr;*/
};
