
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointPather.generated.h"

class USceneComponent;
class USplineComponent;

UCLASS(BlueprintType, hideCategories = (Input, Rendering))
class AWaypointPather : public AActor
{
	GENERATED_BODY()

public:
	AWaypointPather();

	UFUNCTION(BlueprintPure)
	FVector GetPathLocation(float Alpha) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponent;
};
