
#include "UnitGameData.h"
#include "SuperAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitGameData)

const UUnitGameData& UUnitGameData::Get()
{
	return USuperAssetManager::Get().GetGameData();
}
