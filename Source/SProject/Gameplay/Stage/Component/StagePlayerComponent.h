
#pragma once

#include "Components/GameFrameworkComponent.h"

#include "StagePlayerComponent.generated.h"

class APlayerController;
class ULocalPlayer;
class AActor;

UCLASS()
class SPROJECT_API UStagePlayerComponent : public UGameFrameworkComponent
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual APlayerController* GetOwningPlayer() const;

	template <class T>
	T* GetOwningPlayer() const
	{
		return Cast<T>(GetOwningPlayer());
	}

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual ULocalPlayer* GetOwningLocalPlayer() const;

	template <class T>
	T* GetOwningLocalPlayer() const
	{
		return Cast<T>(GetOwningLocalPlayer());
	}

	UFUNCTION(BlueprintCallable, Category = "입력|이벤트")
	void InteractionMouseEvent();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionActor(AActor* HitActor);
};
