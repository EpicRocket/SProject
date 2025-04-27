
#pragma once

#include "Core/Component/GPlayerComponent.h"

#include "PlayerInputComponent.generated.h"

class UInputMappingContext;

UENUM(BlueprintType)
enum class EPlayerInputType : uint8
{
	None = 0,
	Mouse = 1,
};

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UPlayerInputComponent : public UGPlayerComponent
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

public:
	UFUNCTION(BlueprintPure)
	FVector2D GetFirstInputPosition() const;

	UFUNCTION(BlueprintPure)
	FVector2D GetLastInputPosition() const;

protected:
	UFUNCTION()
	void OnControlMappingsRebuilt();

	UFUNCTION(BlueprintImplementableEvent)
	TMap<EPlayerInputType, UInputMappingContext*> GetControlMappings();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionActor(AActor* HitActor);

	UFUNCTION(BlueprintCallable)
	void OnPress();

	UFUNCTION(BlueprintCallable)
	void OnDrag();

	UFUNCTION(BlueprintCallable)
	void OnRelease();

	
protected: // Mouse Events
	void OnMousePress();
	void OnMouseDrag();
	void OnMouseRelease();
private:
	bool bMousePressed = false;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Input")
	EPlayerInputType InputMode = EPlayerInputType::None;

private:
	FVector2D FirstInputPosition = FVector2D::ZeroVector;
	mutable FVector2D LastInputPosition = FVector2D::ZeroVector;

};
