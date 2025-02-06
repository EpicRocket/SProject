
#include "Framework/Player/GPlayerComponent.h"
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
