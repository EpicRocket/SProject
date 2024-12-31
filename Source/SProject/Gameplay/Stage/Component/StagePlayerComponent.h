
#pragma once

#include "Components/GameFrameworkComponent.h"
#include "Framework/Player/GPlayerComponent.h"


#include "StagePlayerComponent.generated.h"

class AActor;

UCLASS()
class SPROJECT_API UStagePlayerComponent : public UGPlayerComponent
{
    GENERATED_BODY()

public:
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
