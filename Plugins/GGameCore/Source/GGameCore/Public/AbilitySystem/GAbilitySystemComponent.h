
#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/GGameplayAbility.h"

#include "GAbilitySystemComponent.generated.h"

UCLASS()
class GGAMECORE_API UGAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:

    typedef TFunctionRef<bool(const UGGameplayAbility*, FGameplayAbilitySpecHandle)> TShouldCancelAbilityFunc;
    void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
};
