
#include "StageSpawner.h"
#include "AIController.h"
#include "Gameplay/ETC/GameplayPathActor.h"
#include "Engine/World.h"
#include "Unit/UnitCharacter.h"
#include "Kismet/GameplayStatics.h"

void AStageSpawner::Spawn()
{
	UE_LOG(LogTemp, Log, TEXT("StageSpawner spawn start."));
	if (!GetWorld()) 
	{
		return;
	}

	TArray<AActor*> GameplayPathActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameplayPathActor::StaticClass(), GameplayPathActors);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (!SpawnUnits.IsEmpty())
	{
		AUnitCharacter* Spawned = GetWorld()->SpawnActor<AUnitCharacter>(SpawnUnits[0], GetActorLocation(), GetActorRotation(), SpawnParams);

		Spawned->SetGenericTeamId(GetGenericTeamId());
	
		AAIController* AIController = GetWorld()->SpawnActor<AAIController>();
		if (AIController)
		{
			AIController->Possess(Cast<APawn>(Spawned));
		}
	}
}

int32 AStageSpawner::GetCurrentWave()
{
	return CurrentWave;
}
