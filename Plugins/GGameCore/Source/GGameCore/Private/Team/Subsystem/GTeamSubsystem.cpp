
#include "Team/Subsystem/GTeamSubsystem.h"
// include Engine
#include "Engine/DataTable.h"
#include "GenericTeamAgentInterface.h"
// include GameCore
#include "Team/GTeamSettings.h"
#include "Team/GTeamTypes.h"
#include "Team/GTeamLoadDataAsset.h"
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

//////////////////////////////////////////////////////////////////////////
// UGTeamSubsystem
//////////////////////////////////////////////////////////////////////////

bool UGTeamSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGTeamSubsystem::RegisterTeams(const TArray<FGTeamTracker>& TeamTrackers, const TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, const TArray<FGForcesRelationshipTableRow>& ForcesRelationshipTableRows)
{
	TArray<TSharedPtr<FGTeam>> Temp;
	for (auto& Tracker : TeamTrackers)
	{
		auto Team = MakeShared<FGTeam>();
		Team->Tracker = Tracker;
		Teams.Emplace(Tracker.ID, Team);
		Forces.FindOrAdd(Tracker.Force).Emplace(Team);
		Temp.Emplace(Team);
	}

	for (auto& Row : RelationshipInForceTableRows)
	{
		auto Force = Forces.Find(Row.Force);
		if (Force)
		{
			for (auto& Team : *Force)
			{
				for (auto& Other : Temp)
				{
					if (Team != Other)
					{
						Team->Relationships.Emplace(Other->Tracker.ID, Row.Relationship);
					}
				}
			}
		}
	}
	
	for (auto& Row : ForcesRelationshipTableRows)
	{
		auto ForceA = Forces.Find(Row.SourceForce);
		auto ForceB = Forces.Find(Row.DestForce);

		if (ForceA && ForceB)
		{
			for (auto& TeamA : *ForceA)
			{
				for (auto& TeamB : *ForceB)
				{
					TeamA->Relationships.Emplace(TeamB->Tracker.ID, Row.Relationship);
				}
			}
		}
	}

	OnRegisterTeams();
}

void UGTeamSubsystem::UnregisterTeams()
{
	Teams.Empty();
	Forces.Empty();
	OnUnregisterTeams();
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

TEnumAsByte<ETeamAttitude::Type> UGTeamSubsystem::GetTeamAttitudeTowards(uint8 Source, uint8 Target) const
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

TEnumAsByte<ETeamAttitude::Type> UGTeamSubsystem::GetAgentAttitudeTowards(TScriptInterface<IGTeamAgent> Source, TScriptInterface<IGTeamAgent> Target) const
{
	return GetTeamAttitudeTowards(Source->GetGenericTeamId(), Target->GetGenericTeamId());
}

//////////////////////////////////////////////////////////////////////////
// UGTeamHelper
//////////////////////////////////////////////////////////////////////////

bool UGTeamHelper::LoadTeamLoadAsset(TSoftObjectPtr<class UGTeamLoadDataAsset> DataAsset, TArray<FGTeamTracker>& TeamTrackers, TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, TArray<FGForcesRelationshipTableRow>& ForcesRelationshipTableRows)
{
	auto Asset = DataAsset.LoadSynchronous();
	if (!Asset)
	{
		return false;
	}

	if (!GTeam::LoadTableRows<FGTeamTracker>(Asset->TeamTracker, TeamTrackers))
	{
		return false;
	}

	if (!GTeam::LoadTableRows<FGRelationshipInForceTableRow>(Asset->RelationshipInForce, RelationshipInForceTableRows))
	{
		return false;
	}

	if (!GTeam::LoadTableRows<FGForcesRelationshipTableRow>(Asset->ForcesRelationship, ForcesRelationshipTableRows))
	{
		return false;
	}

	return true;
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
