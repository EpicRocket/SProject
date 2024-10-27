// This is an automatically generated file. Do not modify it manually.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "EnumTable.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	 = None,
	 = Resource,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemType, EItemType::Max)

UENUM(BlueprintType)
enum class EUseType : uint8
{
	 = None,
	 = GetResource,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUseType, EUseType::Max)

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	 = None,
	 = TowerNomalAttack,
	 = TowerProjectileAttack,
	 = TowerRangeAttack,
	 = TowerSupportAttack,
	 = MeleeAttack,
	 = RangeAttack,
	 = SupportAttack,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttackType, EAttackType::Max)

