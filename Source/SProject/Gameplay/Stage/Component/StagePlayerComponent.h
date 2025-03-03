
#pragma once

#include "Gameplay/Component/GameplayPlayerComponent.h"

#include "StagePlayerComponent.generated.h"

class AActor;
class UGameplayUserPlayer;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UStagePlayerComponent : public UGameplayPlayerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FGErrorInfo NewStart();

	UFUNCTION(BlueprintCallable)
	FGErrorInfo Restart();

	UFUNCTION(BlueprintCallable)
	void SetHp(int32 NewHp);

	UFUNCTION(BlueprintCallable)
	void AddHp(int32 AddHp);

	UFUNCTION(BlueprintPure)
	int32 GetHp() const;

	UFUNCTION(BlueprintCallable)
	void SetUsePoint(int64 NewUsePoint);

	UFUNCTION(BlueprintCallable)
	void AddUsePoint(int64 AddUsePoint);

	UFUNCTION(BlueprintPure)
	int64 GetUsePoint() const;

protected:
	FGErrorInfo SetDefaults();

private:
	bool bMousePressed = false;

	FVector2D FirstMousePressPosition = FVector2D::ZeroVector;
};
