// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/Component/GGameStateComponent.h"
// include Engine
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameStateComponent)

AGameStateBase* UGGameStateComponent::GetGameState() const
{
    return Cast<AGameStateBase>(GetOwner());
}
