#include "AbilitySystem/Attribute/GAttributeSet.h"

#include "AbilitySystem/GAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GAttributeSet)

class UWorld;


UGAttributeSet::UGAttributeSet()
{
}

UWorld* UGAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UGAbilitySystemComponent* UGAttributeSet::GetGAbilitySystemComponent() const
{
	return Cast<UGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
