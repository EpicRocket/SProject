
#include "UnitAbilitySystemComponent.h"
// include Engine
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitAbilitySystemComponent)

//void UUnitAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
//{
//}
//
//void UUnitAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
//{
//}
//
//void UUnitAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
//{
//}
//
//void UUnitAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
//{
//}
//
//void UUnitAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
//{
//}
//
//void UUnitAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
//{
//}
//
//void UUnitAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
//{
//}
//
//void UUnitAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
//{
//}
//
//void UUnitAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
//{
//}
//
//void UUnitAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
//{
//}
//
//void UUnitAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//}
//
//void UUnitAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
//{
//}
//
//void UUnitAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
//{
//}
//
//void UUnitAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
//{
//}
//
//void UUnitAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
//{
//}
//
//void UUnitAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
//{
//}
//
//void UUnitAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
//{
//}
//
//void UUnitAbilitySystemComponent::ClearAbilityInput()
//{
//}
//
//bool UUnitAbilitySystemComponent::IsActivationGroupBlocked(EUnitAbilityActivationGroup Group) const
//{
//    return false;
//}
//
//void UUnitAbilitySystemComponent::AddAbilityToActivationGroup(EUnitAbilityActivationGroup Group, UUnitGameplayAbility* LyraAbility)
//{
//}
//
//void UUnitAbilitySystemComponent::RemoveAbilityFromActivationGroup(EUnitAbilityActivationGroup Group, UUnitGameplayAbility* LyraAbility)
//{
//}
//
//void UUnitAbilitySystemComponent::CancelActivationGroupAbilities(EUnitAbilityActivationGroup Group, UUnitGameplayAbility* IgnoreLyraAbility, bool bReplicateCancelAbility)
//{
//}
//
//void UUnitAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
//{
//}
//
//void UUnitAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
//{
//}
//
//void UUnitAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
//{
//}
//
//void UUnitAbilitySystemComponent::SetTagRelationshipMapping(UUnitAbilityTagRelationshipMapping* NewMapping)
//{
//}
//
//void UUnitAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
//{
//}

bool UUnitAbilitySystemComponent::IsActivationGroupBlocked(EUnitAbilityActivationGroup Group) const
{
	return false;
}
