
#include "Team/Subsystem/GTeamSubsystem.h"
// include Engine
#include "Engine/DataTable.h"
#include "GenericTeamAgentInterface.h"
// include GameCore
#include "Team/GTeamSettings.h"
#include "Team/GTeamTypes.h"
#include "Team/Interface/IGTeamAgent.h"

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

void UGTeamSubsystem::RegisterTeams(const TArray<FGTeamTracker>& TeamTrackers)
{
	Teams.Empty();
	Forces.Empty();

	TArray<TSharedPtr<FGTeam>> Temp;
	for (const FGTeamTracker& Tracker : TeamTrackers)
	{
		auto Team = MakeShared<FGTeam>();
		Team->Tracker = Tracker;
		Teams.Emplace(Tracker.ID, Team);
		Forces.FindOrAdd(Tracker.Force).Emplace(Team);
		Temp.Emplace(Team);
	}

	for (auto& Team : Temp)
	{
		for (auto& Other : Temp)
		{
			if (Team != Other)
			{
				Team->Relationships.Emplace(Other->Tracker.ID, FGTeamRelationship{});
			}
		}
	}
}

FGTeamTracker UGTeamSubsystem::GetTeamTracker(FGenericTeamId TeamID) const
{
	auto Iter = Teams.Find(TeamID);
	if (Iter && (*Iter).IsValid())
	{
		return (*Iter)->Tracker;
	}
	return GetDefault<UGTeamSettings>()->DefaultTeamTracker;
}

TEnumAsByte<ETeamAttitude::Type> UGTeamSubsystem::GetTeamAttitudeTowards(const FGenericTeamId& Source, const FGenericTeamId& Target) const
{
	auto A = Teams.Find(Source);
	auto B = Teams.Find(Target);
	if (A && B)
	{
		auto& Relationships = (*A)->Relationships;
		auto Iter = Relationships.Find(Target);
		if (Iter)
		{
			if ((*Iter).TrustLevel > 0)
			{
				return ETeamAttitude::Friendly;
			}
			else if ((*Iter).TrustLevel < 0)
			{
				return ETeamAttitude::Hostile;
			}
		}
	}
	return ETeamAttitude::Neutral;
}

bool UGTeamHelper::LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& TeamTrackers)
{
	return GTeam::LoadTableRows<FGTeamTracker>(Table, TeamTrackers);
}

bool UGTeamHelper::IsTeamAgentOwner(APlayerController* PC, TScriptInterface<IGTeamAgent> TeamAgent)
{
	IGTeamAgent* Owner = Cast<IGTeamAgent>(PC);
	if (!Owner || !TeamAgent)
	{
		return false;
	}
	return Owner->GetGenericTeamId() == TeamAgent->GetGenericTeamId();
}
