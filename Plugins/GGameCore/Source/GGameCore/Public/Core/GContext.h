
// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GContext.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, Category = "Core", ClassGroup = "Core")
class GGAMECORE_API UGContext : public UObject
{
    GENERATED_BODY()

public:
    virtual void Load() {/* Needs Implement */}
    virtual void Update() {/* Needs Implement */}

};
