
#include "MasterGameLevel.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// include GameCore
#include "Loading/GLoadingManager.h"
// include Project
#include "Gameplay/GameWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MasterGameLevel)

void AMasterGameLevel::BeginPlay()
{
	Super::BeginPlay();

	auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance());
	check(LoadingManager);

	LoadingManager->RegisterLoadingProcessor(this);
}

void AMasterGameLevel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto LoadingManager = UGameInstance::GetSubsystem<UGLoadingManager>(GetGameInstance()))
	{
		LoadingManager->UnregisterLoadingProcessor(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

bool AMasterGameLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	auto GameWorldSubsystem = UWorld::GetSubsystem<UGameWorldSubsystem>(GetWorld());
	if (!GameWorldSubsystem)
	{
		OutReason = TEXT("월드 로드 중");
		return true;
	}

	if (!GameWorldSubsystem->IsExistsLoadedGameWorld())
	{
		OutReason = TEXT("월드 찾는 중");
		return true;
	}

	return false;
}
