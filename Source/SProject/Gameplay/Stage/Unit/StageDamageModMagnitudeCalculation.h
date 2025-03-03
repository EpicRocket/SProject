
#pragma once

#include "GameplayModMagnitudeCalculation.h"

#include "StageDamageModMagnitudeCalculation.generated.h"

UCLASS()
class MY_API UStageDamageModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
