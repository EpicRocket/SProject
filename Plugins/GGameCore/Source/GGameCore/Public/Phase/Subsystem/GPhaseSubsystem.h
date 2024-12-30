
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

#include "GPhaseSubsystem.generated.h"

class UGPhaseGameplayAbility;

DECLARE_DYNAMIC_DELEGATE_OneParam(FGPhaseDynamicDelegate, const UGPhaseGameplayAbility*, Phase);
DECLARE_DELEGATE_OneParam(FGPhaseDelegate, const UGPhaseGameplayAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FGPhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FGPhaseTagDelegate, const FGameplayTag& PhaseTag);

UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
    // An exact match will only receive messages with exactly the same channel
    // (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
    ExactMatch,

    // A partial match will receive any messages rooted in the same channel
    // (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
    PartialMatch
};

USTRUCT()
struct FGPhaseEntry
{
    GENERATED_BODY()

    UPROPERTY()
    FGameplayTag PhaseTag;

    FGPhaseDelegate PhaseEndedCallback;
};

USTRUCT()
struct FGPhaseObserver
{
	GENERATED_BODY()

    UPROPERTY()
    FGameplayTag PhaseTag;

    UPROPERTY()
    EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;

    FGPhaseTagDelegate PhaseCallback;

    bool IsMatch(const FGameplayTag& ComparePhaseTag) const
    {
        switch (MatchType)
        {
        case EPhaseTagMatchType::ExactMatch:    return ComparePhaseTag == PhaseTag;
        case EPhaseTagMatchType::PartialMatch:  return ComparePhaseTag.MatchesTag(PhaseTag);
        }
        return false;
    }
};

UCLASS()
class GGAMECORE_API UGPhaseSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

    friend class UGPhaseGameplayAbility;

protected:
    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

public:
    void StartPhase(TSubclassOf<UGPhaseGameplayAbility> PhaseAbility, FGPhaseDelegate PhaseEnded = FGPhaseDelegate{});

    UFUNCTION(BlueprintCallable, Category = "Phase", meta = (DisplayName="Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<UGPhaseGameplayAbility> PhaseAbility, const FGPhaseDynamicDelegate& PhaseEnded);

    void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FGPhaseTagDelegate& WhenPhaseActive);
    UFUNCTION(BlueprintCallable, Category = "Phase", meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
    void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FGPhaseTagDynamicDelegate WhenPhaseActive);

    void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FGPhaseTagDelegate& WhenPhaseEnd);
    UFUNCTION(BlueprintCallable, Category = "Phase", meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
    void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FGPhaseTagDynamicDelegate WhenPhaseEnd);

    UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "PhaseTag"))
    bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
    void OnBeginPhase(const UGPhaseGameplayAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
    void OnEndPhase(const UGPhaseGameplayAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
    UPROPERTY(Transient)
    TMap<FGameplayAbilitySpecHandle, FGPhaseEntry> ActivePhaseMap;

    UPROPERTY(Transient)
    TArray<FGPhaseObserver> PhaseStartObservers;

	UPROPERTY(Transient)
	TArray<FGPhaseObserver> PhaseEndObservers;
};
