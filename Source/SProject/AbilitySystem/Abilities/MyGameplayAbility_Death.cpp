

#include "MyGameplayAbility_Death.h"

#include "AbilitySystem/MyAbilitySystemLog.h"
#include "AbilitySystem/Abilities/MyGameplayAbility.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
//#include "Character/MyHealthComponent.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameplayAbility_Death)

UMyGameplayAbility_Death::UMyGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	//bAutoStartDeath = true;

	//if (HasAnyFlags(RF_ClassDefaultObject))
	//{
	//	// Add the ability trigger tag as default to the CDO.
	//	FAbilityTriggerData TriggerData;
	//	TriggerData.TriggerTag = MyGameplayTags::GameplayEvent_Death;
	//	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	//	AbilityTriggers.Add(TriggerData);
	//}
}

void UMyGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	//UMyAbilitySystemComponent* MyASC = CastChecked<UMyAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	//FGameplayTagContainer AbilityTypesToIgnore;
	//AbilityTypesToIgnore.AddTag(MyGameplayTags::Ability_Behavior_SurvivesDeath);

	//// Cancel all abilities and block others from starting.
	//MyASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	//SetCanBeCanceled(false);

	//if (!ChangeActivationGroup(EMyAbilityActivationGroup::Exclusive_Blocking))
	//{
	//	UE_LOG(LogMyAbilitySystem, Error, TEXT("UMyGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	//}

	//if (bAutoStartDeath)
	//{
	//	StartDeath();
	//}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMyGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMyGameplayAbility_Death::StartDeath()
{
	/*if (UMyHealthComponent* HealthComponent = UMyHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EMyDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}*/
}

void UMyGameplayAbility_Death::FinishDeath()
{
	/*if (UMyHealthComponent* HealthComponent = UMyHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EMyDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}*/
}

