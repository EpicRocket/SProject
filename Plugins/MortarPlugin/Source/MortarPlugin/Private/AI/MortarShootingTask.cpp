//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarShootingTask.h"
#include "Mortar.h"
#include "AIController.h"
#include "MortarAimingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MortarPlugin.h"


EBTNodeResult::Type UMortarShootingTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();
	//Return The Best TargetPawn
	AActor* TargetPawn = GetBestTarget(DetectedActorsKey.SelectedKeyName, OwnerComp.GetBlackboardComponent(), AIController);


	APawn* BasePawn = AIController->GetPawn();


	if (!TargetPawn || !BasePawn)
	{
		if (BasePawn)
		{

			UMortarAimingComponent* AimingComponent = Cast<UMortarAimingComponent>(BasePawn->GetComponentByClass(UMortarAimingComponent::StaticClass()));
			if (AimingComponent)
			{
				AimingComponent->SetIdle();
			}
		}

		return EBTNodeResult::Failed;
	}



	//Get Base Pawn
	//I.e the owning Actor
	UMortarAimingComponent* AimingComponent = Cast<UMortarAimingComponent>(BasePawn->GetComponentByClass(UMortarAimingComponent::StaticClass()));


	//If we are able to get Aiming COmponent
	if (AimingComponent)
	{
		
		UE_LOG(LogMortarPlugin, Verbose, TEXT("%s Aiming at Target Identified as %s"), *BasePawn->GetName(), *TargetPawn->GetName());
		//Aim at location
		FVector AimLocation;
		FRotator AimRotation;


		TargetPawn->GetActorEyesViewPoint(AimLocation, AimRotation);
		AimingComponent->AimAt(AimLocation);
		AimingComponent->Fire();
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::Failed;
}
