// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "GameplayModMagnitudeCalculation.h"

#include "GModMagnitudeCalculation.generated.h"

class AActor;
struct FGameplayEffectSpec;

UCLASS(Abstract, BlueprintType, Blueprintable, Abstract, Category = "AbilitySystem", ClassGroup = "AbilitySystem")
class GGAMECORE_API UGModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    AActor* GetOriginalInstigator(const FGameplayEffectSpec& Spec) const;
    
    UFUNCTION(BlueprintPure)
    AActor* GetEffectCauser(const FGameplayEffectSpec& Spec) const;

    UFUNCTION(BlueprintPure)
    AActor* GetInstigator(const FGameplayEffectSpec& Spec) const;

    UFUNCTION(BlueprintPure)
    float GetLevel(const FGameplayEffectSpec& Spec) const;

};
