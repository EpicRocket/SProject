
#include "Core/GGameCoreHelper.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
// include GGameCore
#include "Core/Component/GGameStateComponent.h"

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

UGGameStateComponent* UGGameCoreBlueprintFunctionLibrary::GetGameStateComponent(const UObject* WorldContextObject, TSubclassOf<UGGameStateComponent> ComponentClass)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	auto GameState = World->GetGameState();
	if (!GameState)
	{
		return nullptr;
	}

	for (auto Component : GameState->GetComponents())
	{
		if (Component && Component->IsA(ComponentClass))
		{
			return Cast<UGGameStateComponent>(Component);
		}
	}

	return nullptr;
}
