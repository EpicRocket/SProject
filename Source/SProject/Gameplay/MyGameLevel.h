
#pragma once

#include "Engine/LevelScriptActor.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "MyGameLevel.generated.h"

UCLASS(Category = "게임 레벨", notplaceable, meta = (ChildCanTick, KismetHideOverrides = "ReceiveAnyDamage,ReceivePointDamage,ReceiveRadialDamage,ReceiveActorBeginOverlap,ReceiveActorEndOverlap,ReceiveHit,ReceiveDestroyed,ReceiveActorBeginCursorOver,ReceiveActorEndCursorOver,ReceiveActorOnClicked,ReceiveActorOnReleased,ReceiveActorOnInputTouchBegin,ReceiveActorOnInputTouchEnd,ReceiveActorOnInputTouchEnter,ReceiveActorOnInputTouchLeave"), HideCategories = (Collision, Rendering, Transformation))
class MY_API AMyGameLevel : public ALevelScriptActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "로딩")
    virtual bool IsLoading() const;
};
