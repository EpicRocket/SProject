

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
	// ��Ȯ�� ��ġ�ϸ� ä���� ��Ȯ�� ��ġ�ϴ� �޽����� �����մϴ� // (��: "A.B"�� ����ϸ� A.B�� ������� ��ġ������ A.B.C�� ��ġ���� ����).
	ExactMatch,

	// �κ� ��Ī�� ������ ä�ο� ���õ� ��� �޽����� �����մϴ� // (��: "A.B"�� ����ϸ� A.B�� ��ۻӸ� �ƴ϶� A.B.C�� ��۵� ��Ī�˴ϴ�).
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
