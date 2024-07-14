
#include "UnitCharacter.h"
// include Engine
// include Project
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/UnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitCharacter)

FName AUnitCharacter::ASCName(TEXT("AbilitySystemComponent"));
FName AUnitCharacter::UnitSetName(TEXT("UnitSet"));

AUnitCharacter::AUnitCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(AUnitCharacter::ASCName);
	AbilitySystemComponent->SetIsReplicated(false);

	UnitSet = CreateDefaultSubobject<UUnitAttributeSet>(AUnitCharacter::UnitSetName);
}
