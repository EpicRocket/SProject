
#include "UnitGameplayAbility.h"
#include "AbilitySystem/UnitAbilitySystemComponent.h"
#include "Framework/SuperPlayerController.h"
#include "Character/Unit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitGameplayAbility)

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)                                                                              \
	{                                                                                                                                                    \
		if (!ensure(IsInstantiated()))                                                                                                                   \
		{                                                                                                                                                \
			ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName()); \
			return ReturnValue;                                                                                                                          \
		}                                                                                                                                                \
	}

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");

UUnitGameplayAbility::UUnitGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ActivationPolicy = EUnitAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EUnitAbilityActivationGroup::Independent;

	bLogCancelation = false;

	ActiveCameraMode = nullptr;
}

UUnitAbilitySystemComponent* UUnitGameplayAbility::GetUnitAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UUnitAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

ASuperPlayerController* UUnitGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ASuperPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UUnitGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

AUnit* UUnitGameplayAbility::GetUnitFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AUnit>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

