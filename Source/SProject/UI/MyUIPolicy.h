
#pragma once

#include "UI/GUIPolicy.h"

#include "MyUIPolicy.generated.h"

class ULocalPlayer;
class UMyGameLayout;

UCLASS(Abstract, meta = (DisableNativeTick))
class MY_API UMyUIPolicy : public UGUIPolicy
{
	GENERATED_BODY()

};
