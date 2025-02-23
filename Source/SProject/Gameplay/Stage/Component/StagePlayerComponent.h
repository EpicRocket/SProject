
#pragma once

#include "Core/Component/GPlayerComponent.h"

#include "StagePlayerComponent.generated.h"

class AActor;
class UGameplayUserPlayer;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UStagePlayerComponent : public UGPlayerComponent
{
    GENERATED_BODY()

public:
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int32 NewUsePoint);

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

protected:
	UPROPERTY(BlueprintReadWrite, Category = "스테이지|자원")
	int Health;

	UPROPERTY(BlueprintReadWrite, Category = "스테이지|자원")
	int UsePoint;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UGameplayUserPlayer> UserPlayer;

private:
	bool bMousePressed = false;

	FVector2D FirstMousePressPosition = FVector2D::ZeroVector;
};
