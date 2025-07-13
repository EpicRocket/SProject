
#include "StageAIController.h"
// include Engine
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageAIController)

void AStageAIController::StartAI()
{
	if (IsValid(AIBehaviorTree))
	{
		RunBehaviorTree(AIBehaviorTree);
	}
}

void AStageAIController::StopAI()
{
	if (IsValid(BrainComponent))
	{
		BrainComponent->StopLogic("StopAI");
	}
}
