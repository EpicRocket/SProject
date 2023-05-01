//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarTurret.h"
#include "Kismet/KismetMathLibrary.h"

void UMortarTurret::Azimuth(const FVector &RotateDirection)
{

	//---------------------
	FRotator TurretDesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), RotateDirection);
	FRotator TurretDeltaRotation = FMath::RInterpConstantTo(GetComponentRotation(), TurretDesiredRotation,
		                                                    GetWorld()->DeltaTimeSeconds, MaxDegreesPerSecond);
	FRotator TurretOriginalRotation = GetComponentRotation();

	//Only Require To Update Turret Yaw
	TurretOriginalRotation.Yaw = TurretDeltaRotation.Yaw;
	SetWorldRotation(TurretOriginalRotation);


}

void UMortarTurret::SetMaxDegreesPerSecondSpeed(const float& Speed)
{
	MaxDegreesPerSecond = Speed;
}
