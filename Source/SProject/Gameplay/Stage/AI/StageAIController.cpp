
#include "StageAIController.h"
// include Engine
#include "BrainComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageAIController)

void AStageAIController::StartAI()
{
	RunBehaviorTree(AIBehaviorTree);
}

void AStageAIController::StopAI()
{
	BrainComponent->StopLogic("StopAI");
}
