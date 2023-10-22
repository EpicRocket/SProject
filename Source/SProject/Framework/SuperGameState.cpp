
#include "SuperGameState.h"

#include "AbilitySystem/SuperAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SuperGameState)

ASuperGameState::ASuperGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<USuperAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* ASuperGameState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
