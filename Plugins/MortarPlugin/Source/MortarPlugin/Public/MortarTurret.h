//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MortarTurret.generated.h"

/**
 * This is the Middle Part of the Mortar
 * On which the Barrel will be attached. This allows movement in
 * Horizontal direction
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class MORTARPLUGIN_API UMortarTurret : public UStaticMeshComponent
{
	GENERATED_BODY()
public:

	/**
	 * Rotates The Turrets Based on the Direction
	 * @param[in] RotateDirection FVector Telling Which way to look.We will only apply Yaw
	 * @image html Azimuth.jpg Azimuth/Elevation Image
	 */
	void Azimuth(const FVector& RotateDirection);

	/**
	 * Sets MaximumRotation Degree Speed at which the turret can rotate
	 * @param[in]  Speed Maximum Speed of rotation
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Rotation")
	void SetMaxDegreesPerSecondSpeed(const float& Speed);

private:

	/**
	 * Maximum rotation speed of turret. Exposed in Editor
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Rotation")
	float MaxDegreesPerSecond = 120.f;


};
