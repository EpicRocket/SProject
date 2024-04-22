
#pragma once

// Use CommonWidget Plugins...

#include "CommonActivatableWidget.h"

#include "MyGameLayout.generated.h"


enum class EAsyncWidgetLayerState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};

UCLASS(Abstract, Blueprintable, Within = GameUIManagerSubsystem)
class SPROJECT_API UMyGameLayout : public UCommonActivatableWidget
{
    GENERATED_BODY()

};
