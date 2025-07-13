// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/GGameState.h"
// include Engine
#include "Components/GameFrameworkComponentManager.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameState)

FName AGGameState::GS_ASC_NAME = TEXT("GameState ASC");

AGGameState::AGGameState()
{
	AbilitySystemComponent = FObjectInitializer::Get().CreateDefaultSubobject<UGAbilitySystemComponent>(this, AGGameState::GS_ASC_NAME);
	AbilitySystemComponent->SetIsReplicated(bIsReplicated_ASC);
	AbilitySystemComponent->SetReplicationMode(ReplicationMode_ASC);
}

void AGGameState::BeginPlay()
{
	Super::BeginPlay();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AGGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
