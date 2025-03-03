
#pragma once

#include "Core/Component/GPlayerComponent.h"

#include "PlayerInputComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UPlayerInputComponent : public UGPlayerComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void OnInteractionActor(AActor* HitActor);

    UFUNCTION(BlueprintCallable, Category = "입력")
    void InteractionMouseEvent();

    UFUNCTION(BlueprintCallable, Category = "입력")
    void OnMousePressed();

    UFUNCTION(BlueprintCallable, Category = "입력")
    void OnMouseMoved();

    UFUNCTION(BlueprintCallable, Category = "입력")
    void OnMouseReleased();

private:
	bool bMousePressed = false;
	FVector2D FirstMousePressPosition = FVector2D::ZeroVector;

};
