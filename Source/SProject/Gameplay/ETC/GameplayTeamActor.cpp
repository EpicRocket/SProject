
#include "GameplayTeamActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTeamActor)

void AGameplayTeamActor::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
    TeamID = InTeamID;
}

FGenericTeamId AGameplayTeamActor::GetGenericTeamId() const
{
    return TeamID;
}
 