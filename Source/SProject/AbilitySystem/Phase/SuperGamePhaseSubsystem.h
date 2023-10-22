

#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"

#include "SuperGamePhaseSubsystem.generated.h"

template <typename T> class TSubclassOf;

class USuperGamePhaseAbility;

struct FFrame;
struct FGameplayAbilitySpecHandle;

UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
	// 정확히 일치하면 채널이 정확히 일치하는 메시지만 수신합니다 // (예: "A.B"에 등록하면 A.B의 생방송은 일치하지만 A.B.C는 일치하지 않음).
	ExactMatch,

	// 부분 매칭은 동일한 채널에 루팅된 모든 메시지를 수신합니다 // (예: "A.B"에 등록하면 A.B의 방송뿐만 아니라 A.B.C의 방송도 매칭됩니다).
	PartialMatch
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FSuperGamePhaseDynamicDelegate, const USuperGamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FSuperGamePhaseDelegate, const USuperGamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FSuperGamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FSuperGamePhaseTagDelegate, const FGameplayTag& PhaseTag);

UCLASS()
class USuperGamePhaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class USuperGamePhaseAbility;

	struct FGamePhaseEntry
	{
	public:
		FGameplayTag PhaseTag;
		FSuperGamePhaseDelegate PhaseEndedCallback;
	};

	struct FPhaseObserver
	{
	public:
		bool IsMatch(const FGameplayTag& ComparePhaseTag) const;

		FGameplayTag PhaseTag;
		EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
		FSuperGamePhaseTagDelegate PhaseCallback;
	};

public:
	void StartPhase(TSubclassOf<USuperGamePhaseAbility> PhaseAbility, FSuperGamePhaseDelegate PhaseEndedCallback = FSuperGamePhaseDelegate());

	void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FSuperGamePhaseTagDelegate& WhenPhaseActive);

	void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FSuperGamePhaseTagDelegate& WhenPhaseEnd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
	bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "Start Phase", AutoCreateRefTerm = "PhaseEnded"))
	void K2_StartPhase(TSubclassOf<USuperGamePhaseAbility> Phase, const FSuperGamePhaseDynamicDelegate& PhaseEnded);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
	void K2_WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FSuperGamePhaseTagDynamicDelegate WhenPhaseActive);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase", meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
	void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FSuperGamePhaseTagDynamicDelegate WhenPhaseEnd);

	void OnBeginPhase(const USuperGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

	void OnEndPhase(const USuperGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
	TMap<FGameplayAbilitySpecHandle, FGamePhaseEntry> ActivePhaseMap;

	TArray<FPhaseObserver> PhaseStartObservers;

	TArray<FPhaseObserver> PhaseEndObservers;
};
