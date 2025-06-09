// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "MyUserWidget.generated.h"

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, ClassGroup = "UI", meta = (Category = "My UI", DisableNativeTick, DontUseGenericSpawnObject = "true"), MinimalAPI)
class UMyUserWidget : public UCommonUserWidget
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    virtual const FText GetPaletteCategory() override;
#endif

};
