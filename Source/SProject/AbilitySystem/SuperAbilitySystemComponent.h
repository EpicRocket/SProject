
#pragma once

#include "Ability/SuperAbility.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "SuperAbilitySystemComponent.generated.h"

class USuperAbility;

UCLASS()
class SPROJECT_API USuperAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    typedef TFunctionRef<bool(const USuperAbility* Ability, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
    void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
};
