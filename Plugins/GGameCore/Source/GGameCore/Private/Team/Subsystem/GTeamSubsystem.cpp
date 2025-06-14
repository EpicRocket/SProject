
#include "Team/Subsystem/GTeamSubsystem.h"
// include Engine
#include "Engine/DataTable.h"
#include "GenericTeamAgentInterface.h"
// include GameCore
#include "Team/GTeamSettings.h"
#include "Team/GTeamTypes.h"
#include "Team/GTeamLoadDataAsset.h"
#include "Team/GTeamDisplayAsset.h"
#include "Team/Interface/IGTeamAgent.h"
#include "Error/GError.h"
#include "Gameplay/GameplayUserPlayer.h"
#include "Gameplay/GameplayComputerPlayer.h"

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
	auto Flags = EObjectFlags::RF_Transient | EObjectFlags::RF_Public;

	Empty();
	Teams.Reserve(TeamTrackers.Num());
	Forces.Reserve(RelationshipInForceTableRows.Num());

	// NOTE. 팀 생성
	for (auto& Tracker : TeamTrackers)
	{
		auto Team = NewObject<UGTeam>(this, NAME_None, Flags);
		Teams.Emplace(Tracker.ID, Team);
		Forces.Emplace(Tracker.Force, Team);
		Team->Tracker = Tracker;
		Team->DisplayAsset = Tracker.DisplayAssetPtr.LoadSynchronous();
	}

	// NOTE. 포스내 팀 관계 설정
	for (auto& Row : RelationshipInForceTableRows)
	{
		TArray<TWeakObjectPtr<UGTeam>> Force;
		Forces.MultiFind(Row.Force, Force);
			
		for (TWeakObjectPtr<UGTeam> Team : Force)
		{
			if (!Team.IsValid())
			{
				UE_LOG(LogGameError, Warning, TEXT("team object is invalid in %d force"), Row.Force);
				continue;
			}

			for (TWeakObjectPtr<UGTeam> OtherTeam : Force)
			{
				if (Team->Tracker.ID != OtherTeam->Tracker.ID)
				{
					Team->Relationships.Emplace(OtherTeam->Tracker.ID, Row.Relationship);
				}
			}
		}
	}

	// NOTE. 포스간 팀 관계 설정
	for (auto& Row : ForcesRelationshipTableRows)
	{
		TArray<TWeakObjectPtr<UGTeam>> ForceA;
		TArray<TWeakObjectPtr<UGTeam>> ForceB;

		Forces.MultiFind(Row.SourceForce, ForceA);
		Forces.MultiFind(Row.DestForce, ForceB);

		for (TWeakObjectPtr<UGTeam> TeamA : ForceA)
		{
			if (!TeamA.IsValid())
			{
				UE_LOG(LogGameError, Warning, TEXT("team object is invalid in %d source force"), Row.SourceForce);
				continue;
			}
			for (TWeakObjectPtr<UGTeam> TeamB : ForceB)
			{
				if (!TeamB.IsValid())
				{
					UE_LOG(LogGameError, Warning, TEXT("team object is invalid in %d dest force"), Row.DestForce);
					continue;
				}
				if (TeamA->Tracker.ID != TeamB->Tracker.ID)
				{
					TeamA->Relationships.Emplace(TeamB->Tracker.ID, Row.Relationship);
				}
			}
		}
	}

	// NOTE. 게임플레이어 생성
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.ObjectFlags = Flags;

		for (auto& [ID, Team] : Teams)
		{
			AGameplayPlayer* Player = nullptr;
			if (Team->Tracker.PlayerType == EGTeamPlayerType::Player)
			{
				Player = GetWorld()->SpawnActor<AGameplayUserPlayer>(SpawnParams);
			}
			else if (Team->Tracker.PlayerType == EGTeamPlayerType::Computer)
			{
				Player = GetWorld()->SpawnActor<AGameplayComputerPlayer>(SpawnParams);
			}
			else
			{
				continue;
			}

			if (!Player)
			{
				continue;
			}

			Player->SetGenericTeamId(ID);
			Players.Emplace(ID, Player);
		}
	}

	OnRegisterTeams();
}

void UGTeamSubsystem::UnregisterTeams()
{
	Empty();
	OnUnregisterTeams();
}

FGTeamTracker UGTeamSubsystem::GetTeamTracker(FGenericTeamId TeamID) const
{
	auto Iter = Teams.Find(TeamID);
	if (Iter && IsValid(*Iter))
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

uint8 UGTeamSubsystem::IssusePlayerTeamID(APlayerController* PC)
{
	for (auto& [ID, Player] : Players)
	{
		auto User = Cast<AGameplayUserPlayer>(Player);
		if (!IsValid(User))
		{
			continue;
		}

		if (User->OwningPlayerController == nullptr)
		{
			User->OwningPlayerController = PC;
			return ID;
		}
	}

	return 255;
}

AGameplayPlayer* UGTeamSubsystem::GetPlayer(uint8 ID) const
{
	if (Players.Contains(ID))
	{
		return Players[ID];
	}
	return nullptr;
}

void UGTeamSubsystem::Empty()
{
	Teams.Empty();
	Forces.Empty();

	for (auto& [ID, Player] : Players)
	{
		Player->Destroy();
	}
	Players.Empty();
}

//////////////////////////////////////////////////////////////////////////
// UGTeamHelper
//////////////////////////////////////////////////////////////////////////

FGErrorInfo UGTeamHelper::LoadTeamLoadAsset(TSoftObjectPtr<class UGTeamLoadDataAsset> DataAsset, TArray<FGTeamTracker>& TeamTrackers, TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, TArray<FGForcesRelationshipTableRow>& ForcesRelationshipTableRows)
{
	auto Asset = DataAsset.LoadSynchronous();
	if (!Asset)
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("GTeamLoadDataAsset를 찾을 수 없습니다."));
	}

	if (!GTeam::LoadTableRows<FGTeamTracker>(Asset->TeamTracker, TeamTrackers))
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("GTeamTracker 테이블을 찾을 수 없습니다."));
	}

	if (!GTeam::LoadTableRows<FGRelationshipInForceTableRow>(Asset->RelationshipInForce, RelationshipInForceTableRows))
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("FGRelationshipInForceTableRow 테이블을 찾을 수 없습니다."));
	}

	if (!GTeam::LoadTableRows<FGForcesRelationshipTableRow>(Asset->ForcesRelationship, ForcesRelationshipTableRows))
	{
		return GameCore::Throw(GameErr::OBJECT_INVALID, TEXT("FGForcesRelationshipTableRow 테이블을 찾을 수 없습니다."));
	}

	return GameCore::Pass();
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
