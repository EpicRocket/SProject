
#include "MyGameState.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameState)

FName AMyGameState::AbilitySystemComponentName = TEXT("AbilitySystemComponent");

AMyGameState::AMyGameState()
{
	
	AbilitySystemComponent = FObjectInitializer::Get().CreateDefaultSubobject<UMyAbilitySystemComponent>(this, AMyGameState::AbilitySystemComponentName);
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AMyGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
