// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Phase/GPhaseGameplayAbility.h"
// include Engine
#include "AbilitySystemComponent.h"
#include "Engine/GameInstance.h"
// include Project
#include "Phase/Subsystem/GPhaseSubsystem.h"
#include "GMessage/Subsystem/GMessageSubsystem.h"
#include "GMessage/GPhaseMessage.h"

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

			BroadcastPhaseMessage(true);
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

			BroadcastPhaseMessage(false);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGPhaseGameplayAbility::BroadcastPhaseMessage(bool bActive)
{
	auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	auto Subsystem = UGameInstance::GetSubsystem<UGMessageSubsystem>(World->GetGameInstance());
	if (!IsValid(Subsystem))
	{
		return;
	}

	Subsystem->BroadcastMessage<FGPhaseMessage>(
		PhaseTag,
		FGPhaseMessage{ .bActive = bActive }
	);
}
