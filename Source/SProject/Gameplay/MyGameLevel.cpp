
#include "MyGameLevel.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Level.h"
// include GameCore
#include "Loading/GLoadingManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameLevel)

void AMyGameLevel::BeginPlay()
{
	Super::BeginPlay();

	auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance());
	check(LoadingManager);

	LoadingManager->RegisterLoadingProcessor(this);
}

void AMyGameLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance()))
	{
		LoadingManager->UnregisterLoadingProcessor(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool AMyGameLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}

void AMyGameLevel::GetActorsByClass(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors) const
{
	OutActors.Empty();

	auto Level = Cast<ULevel>(GetOuter());
	if (!Level)
	{
		return;
	}

	for (auto& Actor : Level->Actors)
	{
		if (IsValid(Actor) && Actor->IsA(ActorClass))
		{
			OutActors.Add(Actor);
		}
	}
}
