
#include "Team/Subsystem/GTeamSubsystem.h"
// include Engine
#include "Engine/DataTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTeamSubsystem)

bool UGTeamSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGTeamSubsystem::RegisterTeams(const TArray<FGTeamTracker>& InTeams)
{
    Teams.Empty();
	for (const FGTeamTracker& Team : InTeams)
	{
		Teams.Emplace(Team.TeamID, Team);
	}
}

bool UGTeamHelper::LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& Teams)
{
    Teams.Empty();
    if (!IsValid(Table))
    {
        return false;
    }

    FString ContextString;
    TArray<FGTeamTracker*> Rows;
    Table->GetAllRows<FGTeamTracker>(ContextString, Rows);

	for (FGTeamTracker* Row : Rows)
	{
        if (Row)
        {
            Teams.Emplace(*Row);
        }
	}

    return true;
}
