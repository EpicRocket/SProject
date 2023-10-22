
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
			// CDO�� �ƴ� ������ ��� �ν��Ͻ��� ����մϴ�.
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
						UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc: CanBeCanceled��(��) �����̹Ƿ� �ɷ� [%s]��(��) ����� �� �����ϴ�."), *SuperAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// �ν��Ͻ�ȭ���� ���� �����Ƽ CDO�� ����մϴ�.
			if (ShouldCancelFunc(AbilityCDO, AbilitySpec.Handle))
			{
				// �ν��Ͻ�ȭ���� ���� �ɷ��� �������� ����� �� �ֽ��ϴ�.
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}
