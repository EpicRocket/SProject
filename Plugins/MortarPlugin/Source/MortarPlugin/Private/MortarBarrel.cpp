//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarBarrel.h"
#include "Kismet/KismetMathLibrary.h"

void UMortarBarrel::Elevate(const FVector &ElevateDirection)
{

	FRotator BarrelDesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), ElevateDirection);
	FRotator BarrelDeltaRotation = FMath::RInterpConstantTo(GetComponentRotation(), BarrelDesiredRotation, 
		                                                    GetWorld()->DeltaTimeSeconds, MaxDegreesElevationPerSecond);
	FRotator BarrelOriginalRotation = GetComponentRotation();

	//Only Require To Update Barrel Pitch
	float ProcessedElevation = FMath::Clamp<float>(BarrelDeltaRotation.Pitch, MinElevationDegrees, MaxElevationDegrees);
	BarrelOriginalRotation.Pitch = ProcessedElevation;
	SetWorldRotation(BarrelOriginalRotation);
}

void UMortarBarrel::SetMaxDegreesElevationPerSecondSpeed(const float& Speed)
{
	MaxDegreesElevationPerSecond = Speed;
}

void UMortarBarrel::SetMaxElevationDegrees(const float& Degree)
{
	MaxElevationDegrees = Degree;
}

void UMortarBarrel::SetMinElevationDegrees(const float& Degree)
{
	MinElevationDegrees = Degree;
}
