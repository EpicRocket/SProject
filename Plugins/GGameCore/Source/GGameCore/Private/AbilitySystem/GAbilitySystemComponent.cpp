
#include "AbilitySystem/GAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GAbilitySystemComponent)

void UGAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const auto& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		auto GameplayAbility = CastChecked<UGGameplayAbility>(AbilitySpec.Ability);
		if (GameplayAbility->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* Instance : Instances)
			{
				auto AbilityInstance = CastChecked<UGGameplayAbility>(Instance);
				if (ShouldCancelFunc(AbilityInstance, AbilitySpec.Handle))
				{
					if (AbilityInstance->CanBeCanceled())
					{
						AbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), AbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
				}
			}
		}
		else
		{
			if (ShouldCancelFunc(GameplayAbility, AbilitySpec.Handle))
			{
				check(GameplayAbility->CanBeCanceled());
				GameplayAbility->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo{}, bReplicateCancelAbility);
			}
		}
	}
}

bool UGAbilitySystemComponent::IsActivationGroupBlocked(EGAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EGAbilityActivationGroup::Independent:
		// Independent abilities are never blocked.
		bBlocked = false;
		break;

	case EGAbilityActivationGroup::Exclusive_Replaceable:
	case EGAbilityActivationGroup::Exclusive_Blocking:
		// Exclusive abilities can activate if nothing is blocking.
		bBlocked = (ActivationGroupCounts[(uint8)EGAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

void UGAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}
