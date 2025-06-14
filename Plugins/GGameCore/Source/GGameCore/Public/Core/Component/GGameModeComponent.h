// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Core/Component/GGameCoreComponent.h"

#include "GGameModeComponent.generated.h"

class AGameModeBase;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class GGAMECORE_API UGGameModeComponent : public UGGameCoreComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    AGameModeBase* GetGameMode() const;

};
