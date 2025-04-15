// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"

#include "EnumTable.generated.h"


UENUM(BlueprintType)
enum class EAttackType : uint8{
	None = 0 UMETA(DisplayName = "안씀"),
	TowerNomalAttack = 1 UMETA(DisplayName = "즉발형"),
	TowerProjectileAttack = 2 UMETA(DisplayName = "투사체형"),
	TowerRangeAttack = 3 UMETA(DisplayName = "범위 공격형"),
	TowerSupportAttack = 4 UMETA(DisplayName = "지원형"),
	MeleeAttack = 5 UMETA(DisplayName = "근접 공격형"),
	RangeAttack = 6 UMETA(DisplayName = "원거리 공격형"),
	SupportAttack = 7 UMETA(DisplayName = "몬스터 지원형"),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttackType, EAttackType::Max)

UENUM(BlueprintType)
enum class EItemType : uint8{
	None = 0 UMETA(DisplayName = "안씀"),
	Resource = 1 UMETA(DisplayName = "재화"),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemType, EItemType::Max)

UENUM(BlueprintType)
enum class EUseType : uint8{
	None = 0 UMETA(DisplayName = "안씀"),
	GetResource = 1 UMETA(DisplayName = "재화 즉시 획득"),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUseType, EUseType::Max)

