
#include "Core/GGameState.h"
#include "AbilitySystem/GAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameState)

FName AGGameState::GS_ASC_NAME = TEXT("GameState ASC");

AGGameState::AGGameState()
{
	AbilitySystemComponent = FObjectInitializer::Get().CreateDefaultSubobject<UGAbilitySystemComponent>(this, AGGameState::GS_ASC_NAME);
	AbilitySystemComponent->SetIsReplicated(bIsReplicated_ASC);
	AbilitySystemComponent->SetReplicationMode(ReplicationMode_ASC);
}

UAbilitySystemComponent* AGGameState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
