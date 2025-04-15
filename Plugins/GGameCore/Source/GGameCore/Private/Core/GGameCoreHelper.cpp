
#include "Core/GGameCoreHelper.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameCoreHelper)

APlayerController* UGGameCoreBlueprintFunctionLibrary::GetPrimaryPlayerController(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PrimaryController = nullptr;
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* NextPlayer = Iterator->Get();
		if (NextPlayer && NextPlayer->PlayerState && NextPlayer->IsPrimaryPlayer() && NextPlayer->PlayerState->GetUniqueId().IsValid())
		{
			PrimaryController = NextPlayer;
			break;
		}
	}

	return PrimaryController;
}
