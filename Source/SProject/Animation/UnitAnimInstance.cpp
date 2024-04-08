
#include "UnitAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitAnimInstance)

void UUnitAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	if (ensure(ASC))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
}

void UUnitAnimInstance::NativeInitializeAnimation()
{
	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}
