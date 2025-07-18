// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Components/GameFrameworkComponent.h"

#include "GGameCoreComponent.generated.h"

struct FLatentActionInfo;
struct FGErrorInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class GGAMECORE_API UGGameCoreComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UGGameCoreComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	void WaitForInitialize(FLatentActionInfo LatentInfo);

};
