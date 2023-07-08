

#include "CinemachineLockonExtension.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/VirtualCamera/CinemachineLockonFreeLookComponent.h"
#include "Camera/CinemachineTargetGroupComponent.h"
#include "Camera/CinemachineCoreSubSystem.h"
#include "Shared/VectorExtension.h"

void UCinemachineLockonExtension::PostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime)
{
	if (Stage != ECVStage::Finalize)
	{
		return;
	}

	UCinemachineLockonFreeLookComponent* LockonCamera = Cast<UCinemachineLockonFreeLookComponent>(VCamera);
	if (!IsValid(LockonCamera))
	{
		return;
	}

	if (!LockonCamera->IsPossibleLockon())
	{
		return;
	}

	USceneComponent* Seeker = LockonCamera->GetSeeker();
	if (!IsValid(Seeker))
	{
		return;
	}

	const FVector CenterLocation = State.ReferenceLookAt;
	const FVector CameraForward = (CenterLocation - State.RawLocation).GetSafeNormal();
	const FVector SeekerToCenter = (CenterLocation - Seeker->GetComponentLocation()).GetUnsafeNormal();

	float AxisAngle = UVectorExtension::SignedAngle(FVector::ForwardVector, SeekerToCenter, State.ReferenceUp);
	const float Distance = FMath::Min(FVector::Distance(Seeker->GetComponentLocation(), CenterLocation), LockonData.MaxRadius);
	float Alpha = FMath::IsNearlyZero(LockonData.MaxRadius) ? 0.0F : Distance / LockonData.MaxRadius;
	float TargetAngle = FMath::Lerp(LockonData.MinAngle, LockonData.MaxAngle, Alpha);

	TargetAngle = UVectorExtension::IsLeftDirection(SeekerToCenter, (Seeker->GetComponentLocation() - State.RawLocation).GetSafeNormal(), State.ReferenceUp) ? AxisAngle + TargetAngle : AxisAngle - TargetAngle;

	FRotator Result = UKismetMathLibrary::RInterpTo(FRotator(0.0F, LockonCamera->XAxis.Value, 0.0F), FRotator(0.0F, TargetAngle, 0.0F), DeltaTime, LockonData.InterpSpeed);
	LockonCamera->XAxis.Value = Result.Yaw;
}
