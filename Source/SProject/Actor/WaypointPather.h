
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointPather.generated.h"

class USceneComponent;
class USplineComponent;

UCLASS(BlueprintType, hideCategories = (Input, Rendering))
class AWaypointPather : public AActor
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponent;
};
