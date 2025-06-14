// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Core/Component/GPlayerComponent.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GPlayerComponent)

APlayerController* UGPlayerComponent::GetOwningPlayer() const
{
    return Cast<APlayerController>(GetOwner());
}

ULocalPlayer* UGPlayerComponent::GetOwningLocalPlayer() const
{
	return GetOwningPlayer()->GetLocalPlayer();
}
