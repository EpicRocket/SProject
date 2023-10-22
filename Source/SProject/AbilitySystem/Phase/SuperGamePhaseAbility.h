
#pragma once

#include "AbilitySystem/Ability/SuperAbility.h"

#include "SuperGamePhaseAbility.generated.h"

UCLASS(Abstract, HideCategories = Input)
class USuperGamePhaseAbility : public USuperAbility
{
	GENERATED_BODY()

public:
	USuperGamePhaseAbility();

	const FGameplayTag& GetGamePhaseTag() const { return GamePhaseTag; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GamePhase")
	FGameplayTag GamePhaseTag;
};
