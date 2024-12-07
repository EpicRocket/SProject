
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
};
