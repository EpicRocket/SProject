﻿
#pragma once

#include "CoreMinimal.h"
#include "Engine/DPICustomScalingRule.h"

#include "MyDPICustomScalingRule.generated.h"

UCLASS()
class SPROJECT_API UMyDPICustomScalingRule : public UDPICustomScalingRule
{
    GENERATED_BODY()

public:
    virtual float GetDPIScaleBasedOnSize(FIntPoint Size) const override;
};