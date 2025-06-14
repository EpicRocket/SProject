
#include "Gameplay/GameplayPlayer.h"
// include Engine
#include "Engine/World.h"
// include GameCore
#include "Error/GErrorTypes.h"
#include "Team/GTeamTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayPlayer)

AGameplayPlayer::AGameplayPlayer()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameplayPlayer::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID.GetId();
}

FGenericTeamId AGameplayPlayer::GetGenericTeamId() const
{
	return FGenericTeamId(TeamID);
}
