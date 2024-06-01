
#pragma once

#include "UI/PrimaryGameLayout.h"

#include "MyGameLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

UCLASS(Abstract, meta = (DisableNativeTick))
class SPROJECT_API UMyGameLayout : public UPrimaryGameLayout
{
    GENERATED_BODY()

public:
	virtual void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning) override;
};
