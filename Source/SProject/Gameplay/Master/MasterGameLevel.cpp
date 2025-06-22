
#include "MasterGameLevel.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// include GameCore
#include "Loading/GLoadingManager.h"
// include Project
#include "Gameplay/Subsystem/GameplayWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MasterGameLevel)

bool AMasterGameLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	auto GameWorldSubsystem = UWorld::GetSubsystem<UGameplayWorldSubsystem>(GetWorld());
	if (!GameWorldSubsystem)
	{
		OutReason = TEXT("게임 월드 시스템을 로드 중입니다.");
		return true;
	}

	if (!GameWorldSubsystem->IsExistsLoadedGameplayLevel())
	{
		OutReason = TEXT("로드 할 맵을 찾지 못하였습니다.");
		return true;
	}

	return false;
}
