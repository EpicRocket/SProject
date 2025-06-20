// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "GLoadingWidget.generated.h"

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, Category = "Loading", ClassGroup = "Loading", meta = (DisableNativeTick, DontUseGenericSpawnObject = "true"))
class GGAMECORE_API UGLoadingWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// UWidget
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	// ~UWidget

protected:
	// UCommonActivatableWidget
	virtual void NativeOnActivated();
	virtual void NativeOnDeactivated();
	// ~UCommonActivatableWidget

};
