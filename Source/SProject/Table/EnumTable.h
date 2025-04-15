// ?????뵬?? ?癒?짗??곗쨮 ??밴쉐?????뵬??낅빍?? ??롫짗??곗쨮 ??륁젟??? 筌띾뜆苑??

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"

#include "EnumTable.generated.h"


UENUM(BlueprintType)
enum class EAttackType : uint8{
	None = 0 UMETA(DisplayName = "???"),
	TowerNomalAttack = 1 UMETA(DisplayName = "筌앸맧而??),
	TowerProjectileAttack = 2 UMETA(DisplayName = "??沅쀯㎗?꾩굨"),
	TowerRangeAttack = 3 UMETA(DisplayName = "甕곕뗄???⑤벀爰??),
	TowerSupportAttack = 4 UMETA(DisplayName = "筌왖?癒곗굨"),
	MeleeAttack = 5 UMETA(DisplayName = "域뱀눘???⑤벀爰??),
	RangeAttack = 6 UMETA(DisplayName = "?癒?탢???⑤벀爰??),
	SupportAttack = 7 UMETA(DisplayName = "筌뤣딅뮞??筌왖?癒곗굨"),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EAttackType, EAttackType::Max)

UENUM(BlueprintType)
enum class EItemType : uint8{
	None = 0 UMETA(DisplayName = "???"),
	Resource = 1 UMETA(DisplayName = "????),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EItemType, EItemType::Max)

UENUM(BlueprintType)
enum class EUseType : uint8{
	None = 0 UMETA(DisplayName = "???"),
	GetResource = 1 UMETA(DisplayName = "????筌앸맩????얜굣"),
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUseType, EUseType::Max)

