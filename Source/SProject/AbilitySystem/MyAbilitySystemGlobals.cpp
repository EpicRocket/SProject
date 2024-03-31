

#include "MyAbilitySystemGlobals.h"

#include "MyGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyAbilitySystemGlobals)

struct FGameplayEffectContext;

UMyAbilitySystemGlobals::UMyAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UMyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMyGameplayEffectContext();
}

