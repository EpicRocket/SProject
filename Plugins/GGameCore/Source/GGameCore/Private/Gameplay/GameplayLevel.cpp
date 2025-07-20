// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Gameplay/GameplayLevel.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Level.h"
// include GameCore
#include "Loading/GLoadingManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayLevel)

void AGameplayLevel::BeginPlay()
{
	Super::BeginPlay();

	auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance());
	check(LoadingManager);

	LoadingManager->RegisterLoadingProcessor(this);
}

void AGameplayLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance()))
	{
		LoadingManager->UnregisterLoadingProcessor(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool AGameplayLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}

void AGameplayLevel::GetActorsByClass(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors) const
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

TArray<AActor*> AGameplayLevel::GetLevelActors() const
{
	auto Level = Cast<ULevel>(GetOuter());
	if (!Level)
	{
		return TArray<AActor*>{};
	}
	return Level->Actors;
}

TSoftObjectPtr<UWorld> AGameplayLevel::GetLevelSoftObject() const
{
	auto Package = GetPackage();
	if (!Package)
	{
		return TSoftObjectPtr<UWorld>{};
	}

	FString PackageName = Package->GetLoadedPath().GetPackageName();
	FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
	FString SoftPath = PackageName + TEXT(".") + AssetName;

	return TSoftObjectPtr<UWorld>(FSoftObjectPath(SoftPath));
}
