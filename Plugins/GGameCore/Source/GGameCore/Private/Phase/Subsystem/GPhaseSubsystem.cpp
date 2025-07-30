
#include "Phase/Subsystem/GPhaseSubsystem.h"
// include Engine
#include "GameFramework/GameStateBase.h"
#include "AbilitySystemComponent.h"
// include Project
#include "Phase/GPhaseGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GPhaseSubsystem)

DEFINE_LOG_CATEGORY(LogPhase)

bool UGPhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UGPhaseSubsystem::StartPhase(TSubclassOf<UGPhaseGameplayAbility> PhaseAbility, FGPhaseDelegate PhaseEnded)
{
	auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	auto GameState = World->GetGameState();
	if (!IsValid(GameState))
	{
		return;
	}

	auto AbilitySystem = GameState->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	FGameplayAbilitySpec PhaseSpec(PhaseAbility, 1, 0, this);
	FGameplayAbilitySpecHandle SpecHandle = AbilitySystem->GiveAbilityAndActivateOnce(PhaseSpec);
	FGameplayAbilitySpec* FoundSpec = AbilitySystem->FindAbilitySpecFromHandle(SpecHandle);

	if (FoundSpec && FoundSpec->IsActive())
	{
		ActivePhaseMap.FindOrAdd(SpecHandle).PhaseEndedCallback = PhaseEnded;
	}
	else
	{
		PhaseEnded.ExecuteIfBound(nullptr);
	}
}

void UGPhaseSubsystem::K2_StartPhase(TSubclassOf<UGPhaseGameplayAbility> PhaseAbility, const FGPhaseDynamicDelegate& PhaseEnded)
{
	auto EndedDelegate = FGPhaseDelegate::CreateWeakLambda(
		PhaseEnded.GetUObject(),
		[PhaseEnded](auto PhaseAbility) {
			PhaseEnded.ExecuteIfBound(PhaseAbility);
		}
	);

	StartPhase(PhaseAbility, EndedDelegate);
}

void UGPhaseSubsystem::WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, bool bMatchExact, const FGPhaseTagDelegate& WhenPhaseActive)
{
	FGPhaseObserver Observer{
		.PhaseTag = PhaseTag,
		.bMatchExact = bMatchExact,
		.PhaseCallback = WhenPhaseActive
	};

	PhaseStartObservers.Add(Observer);

	if (IsPhaseActive(PhaseTag))
	{
		WhenPhaseActive.ExecuteIfBound(PhaseTag);
	}
}

void UGPhaseSubsystem::K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, bool bMatchExact, FGPhaseTagDynamicDelegate WhenPhaseActive)
{
	auto ActiveDelegate = FGPhaseTagDelegate::CreateWeakLambda(
		WhenPhaseActive.GetUObject(),
		[WhenPhaseActive](auto PhaseTag) {
			WhenPhaseActive.ExecuteIfBound(PhaseTag);
		});

	WhenPhaseStartsOrIsActive(PhaseTag, bMatchExact, ActiveDelegate);
}

void UGPhaseSubsystem::WhenPhaseEnds(FGameplayTag PhaseTag, bool bMatchExact, const FGPhaseTagDelegate& WhenPhaseEnd)
{
	FGPhaseObserver Observer{
		.PhaseTag = PhaseTag,
		.bMatchExact = bMatchExact,
		.PhaseCallback = WhenPhaseEnd
	};

	PhaseEndObservers.Add(Observer);
}

void UGPhaseSubsystem::K2_WhenPhaseEnds(FGameplayTag PhaseTag, bool bMatchExact, FGPhaseTagDynamicDelegate WhenPhaseEnd)
{
	auto EndedDelegate = FGPhaseTagDelegate::CreateWeakLambda(
		WhenPhaseEnd.GetUObject(),
		[WhenPhaseEnd](auto PhaseTag) {
			WhenPhaseEnd.ExecuteIfBound(PhaseTag);
		});

	WhenPhaseEnds(PhaseTag, bMatchExact, EndedDelegate);
}

bool UGPhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
	for (const auto& [_, Entry] : ActivePhaseMap)
	{
		if (Entry.PhaseTag.MatchesTag(PhaseTag))
		{
			return true;
		}
	}
	return false;
}

TArray<FGameplayTag> UGPhaseSubsystem::GetActivePhaseList() const
{
	TArray<FGameplayTag> Array;
	for (const auto& [_, Entry] : ActivePhaseMap)
	{
		Array.Add(Entry.PhaseTag);
	}
	return Array;
}

void UGPhaseSubsystem::OnBeginPhase(const UGPhaseGameplayAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	const auto IncomingPhaseTag = PhaseAbility->PhaseTag;

	auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	auto GameState = World->GetGameState();
	if (!IsValid(GameState))
	{
		return;
	}

	auto AbilitySystem = GameState->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(AbilitySystem))
	{
		return;
	}

	TArray<FGameplayAbilitySpec*> ActivePhases;
	for (const auto& [Handle, _] : ActivePhaseMap)
	{
		if (FGameplayAbilitySpec* Spec = AbilitySystem->FindAbilitySpecFromHandle(Handle))
		{
			ActivePhases.Add(Spec);
		}
	}

	for (auto ActivePhase : ActivePhases)
	{
		const UGPhaseGameplayAbility* ActivePhaseAbility = CastChecked<UGPhaseGameplayAbility>(ActivePhase->Ability);
		const FGameplayTag ActivePhaseTag = ActivePhaseAbility->PhaseTag;

		if (!IncomingPhaseTag.MatchesTag(ActivePhaseTag))
		{
			FGameplayAbilitySpecHandle HandleToEnd = ActivePhase->Handle;
			for (auto& Ability : AbilitySystem->GetActivatableAbilities())
			{
				if (!Ability.IsActive())
				{
					continue;
				}

				for (auto Instance : Ability.GetAbilityInstances())
				{
					if (Ability.Handle != HandleToEnd)
					{
						continue;
					}

					if (Instance->CanBeCanceled())
					{
						Instance->CancelAbility(Ability.Handle, AbilitySystem->AbilityActorInfo.Get(), Instance->GetCurrentActivationInfo(), true);
					}
					else
					{
						UE_LOG(LogPhase, Error, TEXT("%s 어빌리티를 취소 할 수 없습니다."), *Instance->GetName());
					}
				}
			}
		}
	}

	ActivePhaseMap.FindOrAdd(PhaseAbilityHandle).PhaseTag = IncomingPhaseTag;
	
	for (auto& Observer : PhaseStartObservers)
	{
		if (Observer.IsMatch(IncomingPhaseTag))
		{
			Observer.PhaseCallback.ExecuteIfBound(IncomingPhaseTag);
		}
	}
}

void UGPhaseSubsystem::OnEndPhase(const UGPhaseGameplayAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
	const FGameplayTag EndedPhaseTag = PhaseAbility->PhaseTag;

	auto& Entry = ActivePhaseMap.FindChecked(PhaseAbilityHandle);
	Entry.PhaseEndedCallback.ExecuteIfBound(PhaseAbility);

	ActivePhaseMap.Remove(PhaseAbilityHandle);

	for (auto& Observer : PhaseEndObservers)
	{
		if (Observer.IsMatch(EndedPhaseTag))
		{
			Observer.PhaseCallback.ExecuteIfBound(EndedPhaseTag);
		}
	}
}
