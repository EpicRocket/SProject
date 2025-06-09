// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "UI/Button/TextButtonBase.h"

#include "MyTextButtonBase.generated.h"

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, ClassGroup = "UI", meta = (Category = "My UI", DisableNativeTick, DontUseGenericSpawnObject = "true"), MinimalAPI)
class UMyTextButtonBase : public UTextButtonBase
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    virtual const FText GetPaletteCategory() override;
#endif

};
