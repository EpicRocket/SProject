// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "StageTowerUnit.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Gameplay/Stage/Stage.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "Gameplay/Stage/Types/StageTowerTypes.h"
#include "Gameplay/Stage/Unit/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageTowerUnit)

void AStageTowerUnit::OnInit()
{
}

void AStageTowerUnit::OnInitBaseStats()
{
	if (!IsValid(TowerInfoContext))
	{
		UE_LOGFMT(LogStage, Warning, "%s:TowerInfoContext is null", *GetName());
		Remove();
		return;
	}

	TMap<EStageUnitAttribute, double> BaseStats;
	auto Err = UStageTableHelper::GetStageTowerBaseStats(
		TowerInfoContext->TowerInfo.TowerType,
		TowerInfoContext->TowerInfo.Kind,
		TowerInfoContext->TowerInfo.Level,
		BaseStats
	);
	if (!GameCore::IsOK(Err))
	{
		return;
	}

	SetBaseStats(BaseStats);
}


void AStageTowerUnit::Setup(FStageTowerInfo NewTowerInfo)
{
	EObjectFlags Flags = RF_Transient | RF_Public;
	TowerInfoContext = NewObject<UStageTowerContext>(this, NAME_None, Flags);
	TowerInfoContext->TowerInfo = NewTowerInfo;
}

FGErrorInfo AStageTowerUnit::K2_GetTowerInfo(FStageTowerInfo& TowerInfo)
{
	if (!IsValid(TowerInfoContext))
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("%s:TowerInfoContext is null"), *GetName()));
	}

	TowerInfo = TowerInfoContext->TowerInfo;
	return GameCore::Pass();
}

TOptional<FStageTowerInfo> AStageTowerUnit::GetTowerInfo() const
{
	if (!IsValid(TowerInfoContext))
	{
		return TOptional<FStageTowerInfo>{};
	}
	return TowerInfoContext->TowerInfo;
}

FGErrorInfo AStageTowerUnit::K2_GetSellPrice(int32& SellPrice)
{
	if (!IsValid(TowerInfoContext))
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, FString::Printf(TEXT("%s:TowerInfoContext is null"), *GetName()));
	}

	SellPrice = TowerInfoContext->TowerInfo.UsePoint;
	return GameCore::Pass();
}

TOptional<int32> AStageTowerUnit::GetSellPrice() const
{
	if (!IsValid(TowerInfoContext))
	{
		return TOptional<int32>{};
	}
	return TowerInfoContext->TowerInfo.UsePoint;
}
