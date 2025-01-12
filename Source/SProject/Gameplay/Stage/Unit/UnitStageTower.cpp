
#include "UnitStageTower.h"
#include "TableTypes/StageTableTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitStageTower)

EStageTowerType AUnitStageTower::GetTowerType() const
{
    return EStageTowerType::Normal;
}

int32 AUnitStageTower::GetKind() const
{
    return 0;
}

int32 AUnitStageTower::GetLevel() const
{
    return 0;
}

int64 AUnitStageTower::GetSellPrice() const
{
    return 0;
}
