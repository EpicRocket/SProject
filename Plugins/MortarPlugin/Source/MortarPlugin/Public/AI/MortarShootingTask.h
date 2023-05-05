//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortarAIBaseTaskNode.h"
#include "MortarShootingTask.generated.h"

/**
 * Shooting Enemy Task Class This class functionality is to aim and shoot
 * the best target Enemy
 */
UCLASS()
class MORTARPLUGIN_API UMortarShootingTask : public UMortarAIBaseTaskNode
{
	GENERATED_BODY()
	
	/**
	 *  Execute Behavior Tree Task
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/**
	 * Key To use for extracting value of detected actors in ai controller
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|AI")
	struct FBlackboardKeySelector DetectedActorsKey;

};
