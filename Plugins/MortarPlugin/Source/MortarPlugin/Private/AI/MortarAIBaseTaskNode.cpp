//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarAIBaseTaskNode.h"
#include "Mortar.h"
#include "AIController.h"
#include "MortarTeamComponent.h"
#include "MortarHealthComponent.h"
#include "MortarActorsObject.h"
#include "BehaviorTree/BlackboardComponent.h"
   

//Chooses and REturns Best Target 
//If No Actor Fullfills the conditions then nullptr would be returned
//and should be handled in child classes accordingly
AActor* UMortarAIBaseTaskNode::GetBestTarget(const FName& KeyName, UBlackboardComponent* BlackboardComp,
	AAIController* AIController)
{

	if (!BlackboardComp || !AIController)
	{
		return nullptr;
	}

	AMortar* BasePawn = Cast<AMortar>(AIController->GetPawn());


	//Get Base Pawn
	//I.e the owning Actor
	if (BasePawn)
	{
		UMortarHealthComponent* CurrentHealthComponent = Cast<UMortarHealthComponent>(BasePawn->GetComponentByClass(UMortarHealthComponent::StaticClass()));
		UMortarTeamComponent* CurrentTeamComponent = Cast<UMortarTeamComponent>(BasePawn->GetComponentByClass(UMortarTeamComponent::StaticClass()));

		//Check if Owner is Alive
		if (CurrentHealthComponent && !CurrentHealthComponent->IsDead())
		{

			//Get Targets By The KeyName
			UMortarActorsObject* ActorsObject = Cast<UMortarActorsObject>(BlackboardComp->GetValueAsObject(KeyName));

			if (ActorsObject)
			{
				//Get Target Pawns
				TArray<AActor*> TargetPawns = ActorsObject->GetActorsObject();

				//Select The Target Which has Health More than 0
				for (AActor* TargetEnemy : TargetPawns)
				{
					if (!TargetEnemy)
						continue;
					
					//Get Health and Team COmponent of target
					UMortarHealthComponent* TargetHealthComponent = Cast<UMortarHealthComponent>(TargetEnemy->GetComponentByClass(UMortarHealthComponent::StaticClass()));
					UMortarTeamComponent* TargetTeamComponent = Cast<UMortarTeamComponent>(TargetEnemy->GetComponentByClass(UMortarTeamComponent::StaticClass()));


					//Check Team Again 
					if (TargetHealthComponent && !TargetHealthComponent->IsDead() && TargetTeamComponent)
					{
						//Get Attitude towards Each Other
						EMortarAgentAttitude::Type Type = UMortarTeamComponent::GetAttitude(TargetTeamComponent->GetTeamInfo().TeamID,
																							CurrentTeamComponent->GetTeamInfo().TeamID);

						//If Select Enemy is allowed
						if (bSelectEnemy && Type == EMortarAgentAttitude::Hostile)
						{
							//Only Select Actor if it is hostile
							return TargetEnemy;
						}

						//If Select Friends is allowed
						if (bSelectFriends && Type == EMortarAgentAttitude::Friendly)
						{
							//Only Select Actor if it is Friendly
							return TargetEnemy;
						}

						//If Select Neutrals Target
						if (bSelectNeutrals && Type == EMortarAgentAttitude::Neutral)
						{
							//Only Select Actor if it is hostile
							return TargetEnemy;
						}

					}
				}
			}
		}
	}
	return nullptr;
}
