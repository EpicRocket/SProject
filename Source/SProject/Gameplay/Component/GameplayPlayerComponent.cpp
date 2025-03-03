
#include "GameplayPlayerComponent.h"
// include Engine
#include "Engine/World.h"
// include Project
#include "Core/MyPlayerController.h"
#include "Gameplay/Team/GameplayTeamSubsystem.h"
#include "Gameplay/Team/GameplayPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayPlayerComponent)

AGameplayPlayer* UGameplayPlayerComponent::GetGameplayPlayer() const
{
    return Cast<AGameplayPlayer>(GetOwner());
}

void UGameplayPlayerHelper::GetGameplayPlayerComponentByPC(APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent)
{
	FindComponent = nullptr;
	auto MyPC = Cast<AMyPlayerController>(PC);
	if (!MyPC)
	{
		return;
	}

	auto World = MyPC->GetWorld();
	if (!World)
	{
		return;
	}

	auto Subsystem = World->GetSubsystem<UGameplayTeamSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	auto Player = Subsystem->GetPlayer(MyPC->GetTeamID());
	if (!Player)
	{
		return;
	}

	UClass* Com = *ComponentClass;
	FindComponent = Player->GetComponentByClass(Com);
}
