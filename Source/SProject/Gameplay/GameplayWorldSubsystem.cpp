
#include "GameplayWorldSubsystem.h"
// includ Engine
#include "Engine/Engine.h"
#include "AIController.h"
// include Project
#include "Unit/UnitCharacter.h"


#include "GameplayLogging.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayWorldSubsystem)

AUnitCharacter* UGameplayHelper::SpawnUnit(UObject* WorldContextObject, FVector Location, FRotator Rotation, TSubclassOf<AUnitCharacter> UnitClass, TSubclassOf<AAIController> AIController)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	if (!UnitClass)
	{
		return nullptr;
	}

	AUnitCharacter* SpawnUnit = World->SpawnActorDeferred<AUnitCharacter>(UnitClass, FTransform(Rotation, Location), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	FTransform SpawnTransform = FTransform(Rotation, Location);
	SpawnUnit->AIControllerClass = AIController;
	SpawnUnit->FinishSpawning(SpawnTransform);

    return SpawnUnit;
}
