
#pragma once

#include "AbilitySystem/Ability/GGameplayAbility.h"

#include "GPhaseGameplayAbility.generated.h"

UCLASS(Abstract, HideCategories = "Input")
class GGAMECORE_API UGPhaseGameplayAbility : public UGGameplayAbility
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

private:
	void BroadcastPhaseMessage(bool bActive);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag PhaseTag;
};
