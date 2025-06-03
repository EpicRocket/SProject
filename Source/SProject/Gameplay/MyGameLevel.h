
#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "MyGameLevel.generated.h"

UCLASS(notplaceable, meta = (ChildCanTick, KismetHideOverrides = "ReceiveAnyDamage,ReceivePointDamage,ReceiveRadialDamage,ReceiveActorBeginOverlap,ReceiveActorEndOverlap,ReceiveHit,ReceiveDestroyed,ReceiveActorBeginCursorOver,ReceiveActorEndCursorOver,ReceiveActorOnClicked,ReceiveActorOnReleased,ReceiveActorOnInputTouchBegin,ReceiveActorOnInputTouchEnd,ReceiveActorOnInputTouchEnter,ReceiveActorOnInputTouchLeave"), HideCategories = (Collision, Rendering, Transformation))
class MY_API AMyGameLevel : public ALevelScriptActor, public IGLoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
	void GetActorsByClass(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors) const;
};
