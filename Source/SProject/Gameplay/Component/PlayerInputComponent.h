
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

	UFUNCTION(BlueprintCallable)
	void InteractionMouseEvent();

	UFUNCTION(BlueprintCallable)
	void OnMousePressed();

	UFUNCTION(BlueprintCallable)
	void OnMouseMoved();

	UFUNCTION(BlueprintCallable)
	void OnMouseReleased();

	UFUNCTION(BlueprintPure)
	FVector2D GetFirstInputPosition() const;

	UFUNCTION(BlueprintPure)
	FVector2D GetLastInputPosition() const;

private:
	bool bMousePressed = false;
	FVector2D FirstInputPosition = FVector2D::ZeroVector;
	mutable FVector2D LastInputPosition = FVector2D::ZeroVector;

};
