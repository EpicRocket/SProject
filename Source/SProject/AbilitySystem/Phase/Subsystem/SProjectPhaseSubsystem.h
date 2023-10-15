
#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"

#include "SProjectPhaseSubsystem.generated.h"

class USProjectPhaseAbility;

DECLARE_DYNAMIC_DELEGATE_OneParam(FSProjectPhaseDynamicDelegate, const USProjectPhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FSProjectPhaseDelegate, const USProjectPhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FSProjectPhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FSProjectPhaseTagDelegate, const FGameplayTag& PhaseTag);

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

UCLASS()
class USProjectPhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class USProjectPhaseAbility;

	struct FGamePhaseEntry
	{
	public:
		FGameplayTag PhaseTag;
		FSProjectPhaseDelegate PhaseEndedCallback;
	};

	struct FPhaseObserver
	{
	public:
		bool IsMatch(const FGameplayTag& ComparePhaseTag) const;

		FGameplayTag PhaseTag;
		EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
		FSProjectPhaseTagDelegate PhaseCallback;
	};

public:
	void StartPhase(TSubclassOf<USProjectPhaseAbility> PhaseAbility, FSProjectPhaseDelegate PhaseEndedCallback = FSProjectPhaseDelegate());

	void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FSProjectPhaseTagDelegate& WhenPhaseActive);

	void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FSProjectPhaseTagDelegate& WhenPhaseEnd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
	bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<USProjectPhaseAbility> Phase, const FSProjectPhaseDynamicDelegate& PhaseEnded);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
	void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FSProjectPhaseTagDynamicDelegate WhenPhaseActive);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
	void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FSProjectPhaseTagDynamicDelegate WhenPhaseEnd);

	void OnBeginPhase(const USProjectPhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
	void OnEndPhase(const USProjectPhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
	TMap<FGameplayAbilitySpecHandle, FGamePhaseEntry> ActivePhaseMap;

	TArray<FPhaseObserver> PhaseStartObservers;
	TArray<FPhaseObserver> PhaseEndObservers;
};
