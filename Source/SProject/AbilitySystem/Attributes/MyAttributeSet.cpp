

#include "MyAttributeSet.h"

#include "AbilitySystem/MyAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyAttributeSet)

class UWorld;


UMyAttributeSet::UMyAttributeSet()
{
}

UWorld* UMyAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UMyAbilitySystemComponent* UMyAttributeSet::GetMyAbilitySystemComponent() const
{
	return Cast<UMyAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

