
#pragma once

#include "Framework/Player/GPlayerComponent.h"


#include "StagePlayerComponent.generated.h"

class AActor;

UCLASS()
class MY_API UStagePlayerComponent : public UGPlayerComponent
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int32 NewUsePoint);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "스테이지|자원")
	int Health;

	UPROPERTY(BlueprintReadWrite, Category = "스테이지|자원")
	int UsePoint;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionActor(AActor* HitActor);

	UFUNCTION(BlueprintCallable, Category = "입력|이벤트")
	void InteractionMouseEvent();

	UFUNCTION(BlueprintCallable, Category = "입력|이벤트")
	void OnMousePressed();

	UFUNCTION(BlueprintCallable, Category = "입력|이벤트")
	void OnMouseMoved();

	UFUNCTION(BlueprintCallable, Category = "입력|이벤트")
	void OnMouseReleased();

private:
	bool bMousePressed = false;

	FVector2D FirstMousePressPosition = FVector2D::ZeroVector;
};
