
#include "StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageUnitAttributeSet)

UStageUnitAttributeSet::UStageUnitAttributeSet()
	: Level(1.0F)
	, MaxHp(1.0F)
	, Hp(1.0F)
{
}

FGameplayAttribute UStageUnitAttributeHelper::ConvertAttribute(EStageUnitAttribute Attribute)
{
	switch (Attribute)
	{
	case EStageUnitAttribute::Level:
		return UStageUnitAttributeSet::GetLevelAttribute();

	case EStageUnitAttribute::Grade:
		return UStageUnitAttributeSet::GetGradeAttribute();

	case EStageUnitAttribute::Attack:
		return UStageUnitAttributeSet::GetAttackAttribute();

	case EStageUnitAttribute::Defence:
		return UStageUnitAttributeSet::GetDefenceAttribute();

	case EStageUnitAttribute::MaxHp:
		return UStageUnitAttributeSet::GetMaxHpAttribute();

	case EStageUnitAttribute::Hp:
		return UStageUnitAttributeSet::GetHpAttribute();

	case EStageUnitAttribute::AttackSpeed:
		return UStageUnitAttributeSet::GetAttackSpeedAttribute();

	case EStageUnitAttribute::MovementSpeed:
		return UStageUnitAttributeSet::GetMovementSpeedAttribute();

	case EStageUnitAttribute::Range:
		return UStageUnitAttributeSet::GetRangeAttribute();

	case EStageUnitAttribute::SplashScale:
		return UStageUnitAttributeSet::GetSplashScaleAttribute();

	default:
		return FGameplayAttribute{};
	}
}

EStageUnitAttribute UStageUnitAttributeHelper::ConvertAttributeType(FGameplayAttribute Attribute)
{
	if (Attribute == UStageUnitAttributeSet::GetLevelAttribute())
	{
		return EStageUnitAttribute::Level;
	}
	else if (Attribute == UStageUnitAttributeSet::GetGradeAttribute())
	{
		return EStageUnitAttribute::Grade;
	}
	else if (Attribute == UStageUnitAttributeSet::GetAttackAttribute())
	{
		return EStageUnitAttribute::Attack;
	}
	else if (Attribute == UStageUnitAttributeSet::GetDefenceAttribute())
	{
		return EStageUnitAttribute::Defence;
	}
	else if (Attribute == UStageUnitAttributeSet::GetMaxHpAttribute())
	{
		return EStageUnitAttribute::MaxHp;
	}
	else if (Attribute == UStageUnitAttributeSet::GetHpAttribute())
	{
		return EStageUnitAttribute::Hp;
	}
	else if (Attribute == UStageUnitAttributeSet::GetAttackSpeedAttribute())
	{
		return EStageUnitAttribute::AttackSpeed;
	}
	else if (Attribute == UStageUnitAttributeSet::GetMovementSpeedAttribute())
	{
		return EStageUnitAttribute::MovementSpeed;
	}
	else if (Attribute == UStageUnitAttributeSet::GetRangeAttribute())
	{
		return EStageUnitAttribute::Range;
	}
	else if (Attribute == UStageUnitAttributeSet::GetSplashScaleAttribute())
	{
		return EStageUnitAttribute::SplashScale;
	}

	return EStageUnitAttribute::None;
}
