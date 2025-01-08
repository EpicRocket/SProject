
#include "Team/Subsystem/GTeamSubsystem.h"
// include Engine
#include "Engine/DataTable.h"
// include GameCore
#include "Team/GTeamSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTeamSubsystem)

DEFINE_LOG_CATEGORY(LogGTeam);

namespace GTeam
{
    template<typename T = FTableRowBase>
	bool LoadTableRows(UDataTable* Table, TArray<T>& Rows)
	{
		Rows.Empty();
		if (!IsValid(Table))
		{
			return false;
		}
		FString ContextString;
		TArray<T*> TableRows;
		Table->GetAllRows<T>(ContextString, TableRows);
		for (T* Row : TableRows)
		{
			if (Row)
			{
				Rows.Emplace(*Row);
			}
		}
		return true;
	}
}

bool UGTeamSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGTeamSubsystem::RegisterTeams(const TArray<FGTeamTracker>& InTeams, TArray<FGTeamForce> InForces)
{
	if (InForces.IsEmpty())
	{
		FGTeamForce DefaultForce;
		DefaultForce.Attitude = ETeamAttitude::Neutral;
		InForces.Emplace(DefaultForce);
	}

	Teams.Empty();
	TeamForces.Empty();

	for (const FGTeamTracker& Team : InTeams)
	{
		//Teams.Emplace(Team.ID, Team);
	}


}

FGTeamTracker UGTeamSubsystem::GetTeamTracker(FGenericTeamId TeamID) const
{
	return FGTeamTracker{};
    /*auto Iter = Teams.Find(TeamID);
	if (Iter)
	{
		return *Iter;
	}
    return GetDefault<UGTeamSettings>()->DefaultTeamTracker;*/
}

bool UGTeamHelper::LoadTeamForceTableRows(UDataTable* Table, TArray<FGTeamForce>& Forces)
{
	return GTeam::LoadTableRows<FGTeamForce>(Table, Forces);
}

bool UGTeamHelper::LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& Teams)
{
	return GTeam::LoadTableRows<FGTeamTracker>(Table, Teams);
}
