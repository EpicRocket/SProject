
#include "Unit.h"
// include Engine
// include Project
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/UnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Unit)

FName AUnit::ASCName(TEXT("AbilitySystemComponent"));
FName AUnit::UnitSetName(TEXT("UnitSet"));

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(AUnit::ASCName);
	AbilitySystemComponent->SetIsReplicated(false);

	UnitSet = CreateDefaultSubobject<UUnitAttributeSet>(AUnit::UnitSetName);
}
