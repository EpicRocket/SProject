
#include "StageDamageExecutionCalculation.h"

// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Table/ConstTable.h"
#include "Gameplay/Stage/Unit/StageUnitCharacter.h"
#include "Gameplay/Stage/Unit/Attribute/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageDamageExecutionCalculation)

void UStageDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/*
	체력 - (피해량 * 받는 피해 비율)

	받는 피해 비율 = 1 - 피해 흡수량 * 증폭값(가변 난수 값 +-5%)
	피해 흡수량 = 방어력 / (방어력 + 방어상수)
	*/

	//const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!IsValid(SourceASC))
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("GetSourceAbilitySystemComponent"));
		return;
	}

	if (!IsValid(TargetASC))
	{
		GameCore::Throw(GameErr::COMPONENT_INVALID, TEXT("GetTargetAbilitySystemComponent"));
		return;
	}
	bool bFound = false;

	double Damage = SourceASC->GetGameplayAttributeValue(UStageUnitAttributeSet::GetAttackAttribute(), bFound);

	double Defence = TargetASC->GetGameplayAttributeValue(UStageUnitAttributeSet::GetDefenceAttribute(), bFound);
	double DamageAbsorbed = 0.0F;
	if (Defence > 0)
	{
		DamageAbsorbed = Defence / (Defence + GetDefault<UConstSettings>()->DefensiveConst);
	}
	double DamageAmplification = FMath::RandRange(0.95f, 1.05f);
	double DamageRatio = 1 - DamageAbsorbed * DamageAmplification;

	FGameplayModifierEvaluatedData DamageModifier;
	DamageModifier.Attribute = UStageUnitAttributeSet::GetHpAttribute();
	DamageModifier.ModifierOp = EGameplayModOp::Additive;
	DamageModifier.Magnitude = -(Damage * DamageRatio);

	OutExecutionOutput.AddOutputModifier(DamageModifier);
}
