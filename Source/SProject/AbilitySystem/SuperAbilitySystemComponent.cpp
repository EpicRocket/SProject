
#include "SuperAbilitySystemComponent.h"
#include "Ability/SuperAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SuperAbilitySystemComponent)

void USuperAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		USuperAbility* AbilityCDO = CastChecked<USuperAbility>(AbilitySpec.Ability);

		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// CDO가 아닌 스폰된 모든 인스턴스를 취소합니다.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				USuperAbility* SuperAbilityInstance = CastChecked<USuperAbility>(AbilityInstance);

				if (ShouldCancelFunc(SuperAbilityInstance, AbilitySpec.Handle))
				{
					if (SuperAbilityInstance->CanBeCanceled())
					{
						SuperAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), SuperAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc: CanBeCanceled가(가) 거짓이므로 능력 [%s]을(를) 취소할 수 없습니다."), *SuperAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// 인스턴스화되지 않은 어빌리티 CDO를 취소합니다.
			if (ShouldCancelFunc(AbilityCDO, AbilitySpec.Handle))
			{
				// 인스턴스화되지 않은 능력은 언제든지 취소할 수 있습니다.
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}
