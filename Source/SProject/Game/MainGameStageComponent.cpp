
#include "MainGameStageComponent.h"
// include Engine
#include "AbilitySystemComponent.h"
// include Project
#include "Framework/SuperGameState.h"
#include "Actor/WaypointPather.h"
#include "Character/Unit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MainGameStageComponent)


bool FMainGameStageData::IsValidData() const
{
	return 
		IsValid(WaypointPather);
}

//////////////////////////////////////////////////////////////////////////
/// UMainGameStageComponent
//////////////////////////////////////////////////////////////////////////

void UMainGameStageComponent::RegisterGameStage(int32 GameStageId, FMainGameStageData GameStageData)
{
	MainGameStageDataMap.Emplace(GameStageId, GameStageData);
}

const FMainGameStageData& UMainGameStageComponent::GetMainGameStageData(int32 GameStageId) const
{
	if (MainGameStageDataMap.Contains(GameStageId))
	{
		return MainGameStageDataMap[GameStageId];
	}
	else
	{
		return Global::GetEmpty<FMainGameStageData>();
	}
}

void UMainGameStageComponent::SummonUnit(int32 GameStageId, TSubclassOf<AUnit> UnitClass)
{
	if (!UnitClass)
	{
		return;
	}

	if (!MainGameStageDataMap.Contains(GameStageId))
	{
		return;
	}

	ASuperGameState* GameState = GetGameStateChecked<ASuperGameState>();
	if (nullptr == GameState)
	{
		return;
	}

	FMainGameStageData& MainGameStageData = MainGameStageDataMap[GameStageId];
	if (!MainGameStageData.IsValidData())
	{
		return;
	}

	FVector SpawnLocation = MainGameStageData.WaypointPather->GetPathLocation(0.0F);
	FVector NextLocation = MainGameStageData.WaypointPather->GetPathLocation(0.01F);

	FRotator SpawnRotation = FRotationMatrix::MakeFromX(((SpawnLocation - NextLocation).GetSafeNormal())).Rotator();

	AUnit* Unit = GameState->SpawnUnit(SpawnLocation, SpawnRotation, UnitClass);

	if (IsValid(Unit))
	{
		if (UAbilitySystemComponent* AbilitySystem = Unit->GetAbilitySystemComponent())
		{
			OnSpawnedUnit(Unit, AbilitySystem);
		}
		else
		{
			Unit->Destroy();
		}
	}
}
