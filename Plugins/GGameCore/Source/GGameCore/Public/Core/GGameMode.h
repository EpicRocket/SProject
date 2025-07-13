// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GGameMode.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, Transient, hideCategories = (Info, Rendering, MovementReplication, Replication, Actor), Category = "Core", ClassGroup = "Core")
class GGAMECORE_API AGGameMode : public AGameMode
{
    GENERATED_BODY()

protected:
    // AActor
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~AActor
    
};
