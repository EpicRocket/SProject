
#pragma once

#include "AIController.h"

#include "StageAIController.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageAIController : public AAIController
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void StartAI();

	UFUNCTION(BlueprintCallable)
	void StopAI();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<class UBehaviorTree> AIBehaviorTree;

};
