// This is an automatically generated file. Do not modify it manually.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "EnumTypes.generated.h"


UENUM(BlueprintType)
enum class EDomainType : uint8
{
	None = 0,
	CommandCenter = 1,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EDomainType, EDomainType::Max)

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

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0,
	Gold = 1,
	Cash = 2,
	Item = 3,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EResourceType, EResourceType::Max)

