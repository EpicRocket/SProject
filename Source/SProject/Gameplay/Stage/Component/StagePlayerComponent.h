
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
	FGErrorInfo NewStart();

	UFUNCTION(BlueprintCallable)
	FGErrorInfo Restart();

	UFUNCTION(BlueprintCallable)
	void SetHp(int32 NewHp);

	UFUNCTION(BlueprintPure)
	int32 GetHp() const;

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int64 NewUsePoint);

	UFUNCTION(BlueprintPure)
	int64 GetUsePoint() const;

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

	FGErrorInfo SetDefaults();

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UGameplayUserPlayer> UserPlayer;

private:
	bool bMousePressed = false;

	FVector2D FirstMousePressPosition = FVector2D::ZeroVector;
};
