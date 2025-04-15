
#include "Core/Component/GPlayerComponent.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include GameCore
#include "Error/GError.h"
#include "Core/Action/GGameComponentLoadAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GPlayerComponent)

APlayerController* UGPlayerComponent::GetOwningPlayer() const
{
    return Cast<APlayerController>(GetOwner());
}

ULocalPlayer* UGPlayerComponent::GetOwningLocalPlayer() const
{
	return GetOwningPlayer()->GetLocalPlayer();
}
