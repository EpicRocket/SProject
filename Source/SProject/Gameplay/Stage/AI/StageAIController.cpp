
#include "StageAIController.h"
// include Engine
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
// include GameCore
#include "Team/Interface/IGTeamAgent.h"
#include "Gameplay/Stage/ETC/StageSupervisor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageAIController)

void AStageAIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	if (auto TeamAgent = Cast<IGTeamAgent>(GetPawn()))
	{
		TeamAgent->SetGenericTeamId(NewTeamID);
	}
}

void AStageAIController::Setup(AStageSupervisor* InSupervisor, uint8 InTeamID, UBehaviorTree* InAI)
{
	Supervisor = InSupervisor;
	SetGenericTeamId(InTeamID);
	AIBehaviorTree = InAI;
}

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

void AStageAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (auto TeamAgent = Cast<IGTeamAgent>(InPawn))
	{
		TeamAgent->SetGenericTeamId(GetGenericTeamId());
	}
}

void AStageAIController::OnUnPossess()
{
	if (auto TeamAgent = Cast<IGTeamAgent>(GetPawn()))
	{
		TeamAgent->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	Super::OnUnPossess();
}
