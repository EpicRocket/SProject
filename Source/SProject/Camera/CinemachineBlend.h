
#pragma once

#include "CoreMinimal.h"
#include "CinemachineCameraState.h"
#include "CinemachineBlend.generated.h"

class ICinemachineCameraInterface;

UCLASS(BlueprintType)
class UCinemachineBlend : public UObject
{
	GENERATED_BODY()

public:
	UCinemachineBlend();

	UCinemachineBlend(ICinemachineCameraInterface* InCameraA, ICinemachineCameraInterface* InCameraB, const FRuntimeFloatCurve& Curve, float InTimeInBlend, float InDuration);

	float GetBlendWeight() const;

	bool IsValid() const;

	bool IsComplete() const;

	FString GetDescription() const;

	bool Uses(ICinemachineCameraInterface* ICamera);

	void UpdateCameraState(FVector WorldUp, float DeltaTime);

	FCinemachineCameraState GetState() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	ICinemachineCameraInterface* CameraA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	ICinemachineCameraInterface* CameraB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	FRuntimeFloatCurve BlendCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	float TimeInBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	float Duraction;
};
