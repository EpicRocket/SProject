
#pragma once

#include "Engine/World.h"

#include "GUIPolicy.generated.h"

class UGLocalPlayer;

UCLASS(Abstract, Blueprintable, Within = GUIManagerSubsystem)
class GGAMECORE_API UGUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	void NotifyPlayerAdded(UGLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer);
};
