//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "MortarBarrel.generated.h"


/**
 * This is the top most Part of the Mortar From where
 * The Projectile will actually fire. This mesh can move
 * Vertically only
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class MORTARPLUGIN_API UMortarBarrel : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	/**
	 *   Function to Elevate the barrel based on the speed mentioned based on The Direction Specified
	 *   @param[in] ElevateDirection FVector Containing Direction In which Barrel has to elevate
	 */
	void Elevate(const FVector& ElevateDirection);

	/**
	 *   Function to Set Maximum Elevation speed Per Second
	 *   @param[in] ElevationSpeed Maximum Elevation Speed
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Elevation")
	void SetMaxDegreesElevationPerSecondSpeed(const float& ElevationSpeed);

	/**
	 *   Function to Set Maximum Elevation Degrees
	 *   @param[in] MaxElevationDegree Maximum Degree to Set for Elevation
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Elevation")
	void SetMaxElevationDegrees(const float& MaxElevationDegree);

	/**
	 *   Function to Set Minimum Elevation Degrees
	 *   @param[in] MinElevationDegree Minimum Degree to Set for Elevation
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Elevation")
	void SetMinElevationDegrees(const float& MinElevationDegree);

private:

	/**
	 *	Maximum Elevation speed of barrel
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Elevation")
	float MaxDegreesElevationPerSecond = 120.f;

	/**
	 *	Maximum Elevation of barrel
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Elevation")
	float MaxElevationDegrees = 70.f;

	/**
	 *	Minimum Elevation of barrel
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Elevation")
	float MinElevationDegrees = -30.f;

};
