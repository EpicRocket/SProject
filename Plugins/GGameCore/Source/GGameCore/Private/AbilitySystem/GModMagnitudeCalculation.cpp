// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "AbilitySystem/GModMagnitudeCalculation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GModMagnitudeCalculation)

AActor *UGModMagnitudeCalculation::GetOriginalInstigator(const FGameplayEffectSpec& Spec) const
{
    return Spec.GetContext().GetOriginalInstigator();
}

AActor *UGModMagnitudeCalculation::GetEffectCauser(const FGameplayEffectSpec& Spec) const
{
    return Spec.GetContext().GetEffectCauser();
}

AActor *UGModMagnitudeCalculation::GetInstigator(const FGameplayEffectSpec& Spec) const
{
    return Spec.GetContext().GetInstigator();
}

float UGModMagnitudeCalculation::GetLevel(const FGameplayEffectSpec& Spec) const
{
    return Spec.GetLevel();
}
