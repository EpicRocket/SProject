//Copyright(c) 2021 FinalRockstar All Rights Reserved

#include "MortarAIController.h"
#include "BrainComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MortarActorsObject.h"
#include "Mortar.h"
#include "MortarTeamComponent.h"
#include "MortarPlugin.h"


//Called in begining
void AMortarAIController::BeginPlay()
{
	Super::BeginPlay();

	//Ensure Both Blackboard and behavior tree are setup
	if (!ensure(MortarBlackboard) || !ensure(MortarBehaviorTree))
	{
		return;
	}

	//Specify Blackboard
	UseBlackboard(MortarBlackboard, MortarBlackboardComponent);

	// Run the behavior tree
	RunBehaviorTree(MortarBehaviorTree);

	ActorsObject = NewObject< UMortarActorsObject>();

	//Find AI Perception from Blue Print
	MortarAIPerception = FindComponentByClass<UAIPerceptionComponent>();
	if (!ensure(MortarAIPerception))
	{
		return;
	}

	//Function that will be called When Perception Updates
	MortarAIPerception->OnPerceptionUpdated.AddDynamic(this, &AMortarAIController::SenseAllActors);

}

//Registers Pawn
void AMortarAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (InPawn)
	{
		AMortar* PossessedMortar = Cast<AMortar>(InPawn);

		//Pawn Is Different
		if (!ensure(PossessedMortar))
		{
			return;
		}

		//Registers Events

		//On Death Event
		PossessedMortar->OnMortarDeath.AddUniqueDynamic(this, &AMortarAIController::OnDeathDelegate);
		//On Pause event
		PossessedMortar->OnMortarPause.AddUniqueDynamic(this, &AMortarAIController::OnPauseDelegate);
		//On Resume Event
		PossessedMortar->OnMortarResume.AddUniqueDynamic(this, &AMortarAIController::OnResumeDelegate);

		//Set Team Info
		UMortarTeamComponent* teamComponent = Cast<UMortarTeamComponent>(PossessedMortar->GetComponentByClass(UMortarTeamComponent::StaticClass()));

		if (teamComponent)
		{
			OwnerTeamID = teamComponent->GetTeamInfo().TeamID;
			SetGenericTeamId(OwnerTeamID);
		}

	}
}

//Sense all Actors by the ai and sort them accordingly
void AMortarAIController::SenseAllActors(const TArray<AActor*>& DetectedActors)
{
	MortarBlackboardComponent->ClearValue(EnemyKey);
	if (!GetPawn() || !ActorsObject)
	{
		return;
	}

	TArray<AActor*> BestActors, SensedActors;
	//Get all the actors currently in detection area of AI
	MortarAIPerception->GetCurrentlyPerceivedActors(nullptr, SensedActors);


	//Sort Targets Based on Some condition By default it is distance
	BestActors = GetBestTarget(SensedActors);

	UE_LOG(LogMortarPlugin, Verbose, TEXT("%s Sensed Total Actors %d"), *GetPawn()->GetName(), BestActors.Num());
	if (BestActors.Num() > 0)
	{
		//Set Blackboard Key
		ActorsObject->SetActorsObject(BestActors);
		MortarBlackboardComponent->SetValueAsObject(EnemyKey, ActorsObject);
	}
	else
	{
		UE_LOG(LogMortarPlugin, Error, TEXT("Key I am clearing for %s"),*GetPawn()->GetName());
		//Clear Blackboard Key
		MortarBlackboardComponent->ClearValue(EnemyKey);
	}
}

//Sort Array Based on Distance
TArray<AActor*> AMortarAIController::GetBestTarget_Implementation(const TArray<AActor*>& DetectedActors)
{
	TArray <AActor*>BestActors = DetectedActors;
	APawn* OwnerPawn = GetPawn();

	if (OwnerPawn)
	{
		//Sort Based on the predicate defined
		FVector OwnerLocation = OwnerPawn->GetActorLocation();
		Algo::Sort(BestActors, FMortarSortByDistance(OwnerLocation));
	}

	return BestActors;
}


//On Destory Disconnect the controller and mark for destroying
void AMortarAIController::OnDeathDelegate()
{

	if (GetPawn())
	{
		GetPawn()->DetachFromControllerPendingDestroy();
	}
}

//We will stop the logic of Behavior Tree
void AMortarAIController::OnPauseDelegate()
{
	if (GetBrainComponent())
	{
		GetBrainComponent()->StopLogic("Owner Dead Waiting to Respawn!");
	}
}

//We will restart the logic of Behavior Tree
void AMortarAIController::OnResumeDelegate()
{
	if (GetBrainComponent())
	{
		GetBrainComponent()->RestartLogic();
	}
}

//Setting behavior of our AI Pawn
ETeamAttitude::Type AMortarAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{

		UMortarTeamComponent* OtherPlayerTeamComponent = Cast<UMortarTeamComponent>(OtherPawn->GetComponentByClass(UMortarTeamComponent::StaticClass()));

		if (OtherPlayerTeamComponent)
		{
			//Simple Cast
			ETeamAttitude::Type Type = (ETeamAttitude::Type)(UMortarTeamComponent::GetAttitude(OtherPlayerTeamComponent->GetTeamInfo().TeamID,
																								OwnerTeamID));

			return Type;
		}

	}

	return ETeamAttitude::Neutral;
}
