// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Loading/Interface/IGLoadingProcess.h"

#include "GameplayLevel.generated.h"

class ULevel;

UCLASS(notplaceable, Category = "Gameplay", ClassGroup = "Gameplay", meta = (ChildCanTick, KismetHideOverrides = "ReceiveAnyDamage,ReceivePointDamage,ReceiveRadialDamage,ReceiveActorBeginOverlap,ReceiveActorEndOverlap,ReceiveHit,ReceiveDestroyed,ReceiveActorBeginCursorOver,ReceiveActorEndCursorOver,ReceiveActorOnClicked,ReceiveActorOnReleased,ReceiveActorOnInputTouchBegin,ReceiveActorOnInputTouchEnd,ReceiveActorOnInputTouchEnter,ReceiveActorOnInputTouchLeave"), HideCategories = (Collision, Rendering, Transformation))
class GGAMECORE_API AGameplayLevel : public ALevelScriptActor, public IGLoadingProcess
{
    GENERATED_BODY()

public:
    // AActor
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // ~AActor

    // IGLoadingProcess
    virtual bool ShouldShowLoadingScreen(FString &OutReason) const override;
    // ~IGLoadingProcess

    UFUNCTION(BlueprintPure, Category = "Gameplay", meta = (DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
    void GetActorsByClass(TSubclassOf<AActor> ActorClass, TArray<AActor*> &OutActors) const;
    
    UFUNCTION(BlueprintPure, Category = "Gameplay")
    TArray<AActor*> GetLevelActors() const;

    UFUNCTION(BlueprintPure)
    TSoftObjectPtr<UWorld> GetLevelSoftObject() const;

};
