
#include "Gameplay/Component/GameplayPlayerComponent.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Team/Subsystem/GTeamSubsystem.h"
#include "Gameplay/GameplayPlayer.h"
#include "Error/GError.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayPlayerComponent)

AGameplayPlayer* UGameplayPlayerComponent::GetGameplayPlayer() const
{
	return Cast<AGameplayPlayer>(GetOwner());
}

FGErrorInfo UGameplayPlayerHelper::GetGameplayPlayerComponentByPC(APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent)
{
	FindComponent = nullptr;
	if (!IsValid(PC))
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("APlayerController"));
	}

	auto World = PC->GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("PC"));
	}

	auto Subsystem = World->GetSubsystem<UGTeamSubsystem>();
	if (!Subsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGTeamSubsystem"));
	}

	IGTeamAgent* TeamAgent = Cast<IGTeamAgent>(PC);
	if (!TeamAgent)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("IGTeamAgent Cast failure"));
	}

	auto Player = Subsystem->GetPlayer(TeamAgent->GetTeamID());
	if (!Player)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("GameplayPlayer isn't found"));
	}

	UClass* Com = *ComponentClass;
	FindComponent = Player->GetComponentByClass(Com);

	return GameCore::Pass();
}
