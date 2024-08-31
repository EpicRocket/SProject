
#pragma once

#include "AbilitySystem/GGameplayAbility.h"

#include "GPhaseGameplayAbility.generated.h"

UCLASS(Abstract, HideCategories = "Input")
class GGAMECORE_API UGPhaseGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGPhaseGameplayAbility();

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "페이즈")
    FGameplayTag PhaseTag;
};
