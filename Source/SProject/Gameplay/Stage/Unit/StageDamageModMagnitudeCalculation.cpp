
#include "StageDamageModMagnitudeCalculation.h"

#include "Gameplay/Stage/Unit/StageUnitCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageDamageModMagnitudeCalculation)

float UStageDamageModMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    auto A =Spec.GetContext().GetOriginalInstigator();
    auto B = Spec.GetContext().GetEffectCauser();
    auto C = Spec.GetContext().GetInstigator();

    return 0.0F;
}
