// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/Component/GGameModeComponent.h"
// include Engine
#include "GameFramework/GameModeBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GGameModeComponent)

AGameModeBase* UGGameModeComponent::GetGameMode() const
{
	return Cast<AGameModeBase>(GetOwner());
}
