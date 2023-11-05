
#include "Unit.h"
// include Engine

// include Project
#include "AbilitySystem/SuperAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Unit)


AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<USuperAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AUnit::BeginDestroy()
{
	Super::BeginDestroy();


}

UAbilitySystemComponent* AUnit::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUnit::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
}

void AUnit::UnitInitailize()
{
	UnitId = FGuid::NewGuid();
}

// Utility
bool IsValidUnit(AUnit* Unit)
{
	return IsValid(Unit) && Unit->UnitId.IsValid();
}
