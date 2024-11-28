
#include "Phase/GPhaseGameplayAbility.h"
// include Engine
#include "AbilitySystemComponent.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
// include Project
#include "Phase/Subsystem/GPhaseSubsystem.h"
#include "GameplayMessage/GPhaseMessage.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GPhaseGameplayAbility)

UGPhaseGameplayAbility::UGPhaseGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

#if WITH_EDITOR
EDataValidationResult UGPhaseGameplayAbility::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!PhaseTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::FromString(TEXT("페이즈 태그가 없습니다.")));
	}

	return Result;
}
#endif

void UGPhaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo->IsNetAuthority())
	{
		if(auto AbilitySystem = ActorInfo->AbilitySystemComponent; AbilitySystem.IsValid())
		{
			if (auto World = AbilitySystem->GetWorld())
			{
				if (auto Subsystem = World->GetSubsystem<UGPhaseSubsystem>(); IsValid(Subsystem))
				{
					Subsystem->OnBeginPhase(this, Handle);
				}
			}

			UGameplayMessageSubsystem::Get(AbilitySystem.Get()).BroadcastMessage<FGPhaseMessage>(
				PhaseTag,
				FGPhaseMessage{ .bActive = true }
			);
		}
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGPhaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (auto AbilitySystem = ActorInfo->AbilitySystemComponent; AbilitySystem.IsValid())
		{
			if (auto World = AbilitySystem->GetWorld(); IsValid(World))
			{
				if (auto Subsystem = World->GetSubsystem<UGPhaseSubsystem>(); IsValid(Subsystem))
				{
					Subsystem->OnBeginPhase(this, Handle);
				}
			}

			UGameplayMessageSubsystem::Get(AbilitySystem.Get()).BroadcastMessage<FGPhaseMessage>(
				PhaseTag,
				FGPhaseMessage{ .bActive = false }
			);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}