
#include "Team/Subsystem/GTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTeamSubsystem)

bool UGTeamSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGTeamSubsystem::RegisterTeams(const TArray<FGTeamTracker>& InTeams)
{
}

void UGTeamHelper::LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& Teams)
{
}
