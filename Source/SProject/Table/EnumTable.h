// This is an automatically generated file. Do not modify it manually. [2024.10.27-17.37.19]
#pragma once

#include "CoreMinimal.h"

#include "Misc/EnumRange.h"
#include "EnumTable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Gold = 1,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemType, EItemType::Max)

UENUM(BlueprintType)
enum class EUseType : uint8
{
	None = 0,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUseType, EUseType::Max)

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None = 0,
	TowerNomalAttack = 1,
	TowerProjectileAttack = 2,
	TowerRangeAttack = 3,
	TowerSupportAttack = 4,
	MeleeAttack = 5,
	RangeAttack = 6,
	SupportAttack = 7,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttackType, EAttackType::Max)
