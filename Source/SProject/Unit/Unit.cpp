
#include "Unit.h"
// include Engine
#include "AbilitySystem/MyAbilitySystemComponent.h"
// include Project

#include UE_INLINE_GENERATED_CPP_BY_NAME(Unit)

FName AUnit::ASCName(TEXT("AbilitySystemComponent"));

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(AUnit::ASCName);
	AbilitySystemComponent->SetIsReplicated(false);
}
