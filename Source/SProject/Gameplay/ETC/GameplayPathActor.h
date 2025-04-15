
#pragma once

#include "GameFramework/Actor.h"
#include "Gameplay/Interface/IGameplayActorTag.h"

#include "GameplayPathActor.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AGameplayPathActor : public AActor, public IGameplayActorTag
{
    GENERATED_BODY()

public:
	static const FName PathTagName;

public:
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, meta = (CompactNodeTitle = "Spline"))
	class USplineComponent* GetSpline() const;

	UFUNCTION(BlueprintPure)
	FVector GetLocationAtInputKey(float InputKey) const;

	UFUNCTION(BlueprintPure)
	float GetClosestInputKey(const FVector& Location) const;

	UFUNCTION(BlueprintPure)
	bool IsAtEnd(const FVector& Location, float Radius);

};
