
#include "Core/GGameCoreHelper.h"
// include Engine
#include "Engine/Gameinstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
// include GGameCore
#include "Core/Component/GGameStateComponent.h"
#include "Core/Component/GGameModeComponent.h"
#include "Core/Component/GPlayerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameCoreHelper)

ULocalPlayer* UGGameCoreHelper::GetPrimaryLocalPlayer(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	auto World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	auto GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetFirstGamePlayer();
}

APlayerController* UGGameCoreHelper::GetPrimaryPlayerController(UObject* WorldContextObject)
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

UGGameModeComponent* UGGameCoreHelper::GetGameModeComponent(const UObject* WorldContextObject, TSubclassOf<UGGameModeComponent> ComponentClass)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	auto GameMode = World->GetAuthGameMode();
	if (!GameMode)
	{
		return nullptr;
	}

	for (auto Component : GameMode->GetComponents())
	{
		if (Component && Component->IsA(ComponentClass))
		{
			return Cast<UGGameModeComponent>(Component);
		}
	}

	return nullptr;
}

UGGameStateComponent* UGGameCoreHelper::GetGameStateComponent(const UObject* WorldContextObject, TSubclassOf<UGGameStateComponent> ComponentClass)
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

UGPlayerComponent* UGGameCoreHelper::GetPlayerControllerComponent(const APlayerController* PlayerController, TSubclassOf<UGPlayerComponent> ComponentClass)
{
	if (!PlayerController)
	{
		return nullptr;
	}

	for (auto Component : PlayerController->GetComponents())
	{
		if (Component && Component->IsA(ComponentClass))
		{
			return Cast<UGPlayerComponent>(Component);
		}
	}

	return nullptr;
}
