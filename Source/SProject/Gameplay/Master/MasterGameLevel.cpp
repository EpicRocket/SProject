
#include "MasterGameLevel.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// include GameCore
#include "Loading/GLoadingManager.h"
// include Project
#include "Gameplay/GameWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MasterGameLevel)

bool AMasterGameLevel::ShouldShowLoadingScreen(FString& OutReason) const
{
	auto GameWorldSubsystem = UWorld::GetSubsystem<UGameWorldSubsystem>(GetWorld());
	if (!GameWorldSubsystem)
	{
		OutReason = TEXT("?붾뱶 濡쒕뱶 以?);
		return true;
	}

	if (!GameWorldSubsystem->IsExistsLoadedGameWorld())
	{
		OutReason = TEXT("?붾뱶 李얜뒗 以?);
		return true;
	}

	return false;
}
