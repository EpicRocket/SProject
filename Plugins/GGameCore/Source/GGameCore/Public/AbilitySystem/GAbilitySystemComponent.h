
#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/GGameplayAbility.h"
#include "NativeGameplayTags.h"

#include "GAbilitySystemComponent.generated.h"

class AActor;
class UGameplayAbility;
class UObject;
struct FFrame;
struct FGameplayAbilityTargetDataHandle;

UCLASS()
class GGAMECORE_API UGAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:

    typedef TFunctionRef<bool(const UGGameplayAbility*, FGameplayAbilitySpecHandle)> TShouldCancelAbilityFunc;
    void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

    bool IsActivationGroupBlocked(EGAbilityActivationGroup Group) const;
    void AddAbilityToActivationGroup(EGAbilityActivationGroup Group, UGGameplayAbility* LyraAbility);
    void RemoveAbilityFromActivationGroup(EGAbilityActivationGroup Group, UGGameplayAbility* LyraAbility);
    void CancelActivationGroupAbilities(EGAbilityActivationGroup Group, UGGameplayAbility* IgnoreLyraAbility, bool bReplicateCancelAbility);

    /** Gets the ability target data associated with the given ability handle and activation info */
    void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);


protected:
    int32 ActivationGroupCounts[(uint8)EGAbilityActivationGroup::MAX];
};
