
#include "GameplayTeamActor.h"
// include GameCore
#include "Core/GPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTeamActor)

void AGameplayTeamActor::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
    TeamID = InTeamID.GetId();
}

FGenericTeamId AGameplayTeamActor::GetGenericTeamId() const
{
    return FGenericTeamId(TeamID);
}
