
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
