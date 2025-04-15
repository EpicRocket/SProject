
#include "StageDamageExecutionCalculation.h"

// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Table/ConstTable.h"
#include "Gameplay/Stage/Unit/StageUnitCharacter.h"
#include "Gameplay/Stage/Unit/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageDamageExecutionCalculation)

void UStageDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/*
	泥대젰 - (?쇳빐??* 諛쏅뒗 ?쇳빐 鍮꾩쑉)

	諛쏅뒗 ?쇳빐 鍮꾩쑉 = 1 - ?쇳빐 ?≪닔??* 利앺룺媛?媛蹂 ?쒖닔 媛?+-5%)
	?쇳빐 ?≪닔??= 諛⑹뼱??/ (諛⑹뼱??+ 諛⑹뼱?곸닔)
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
