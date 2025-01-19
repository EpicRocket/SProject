
#include "UnitStageTower.h"

#include "TableTypes/StageTableTypes.h"
#include "TableRepository/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitStageTower)

EStageTowerType AUnitStageTower::GetTowerType() const
{
    return GetBuildReceipt()->TowerType;
}

int32 AUnitStageTower::GetKind() const
{
    return GetBuildReceipt()->Kind;
}

int32 AUnitStageTower::GetLevel() const
{
    return GetBuildReceipt()->Level;
}

int64 AUnitStageTower::GetSellPrice() const
{
	int64 Result = 0;
	if (!UStageTableHelper::GetStageTowerSellPrice(GetTowerType(), GetKind(), GetLevel(), Result))
	{
		return 0;
	}
	return Result;
}

void AUnitStageTower::SetBuildReceipt(const FBuildStageTower& Tower)
{
	*(GetBuildReceipt()) = Tower;
}

TSharedRef<FBuildStageTower> AUnitStageTower::GetBuildReceipt() const
{
	if (!BuildReceipt.IsValid())
	{
		BuildReceipt = MakeShared<FBuildStageTower>();
	}
    return BuildReceipt.ToSharedRef();
}
