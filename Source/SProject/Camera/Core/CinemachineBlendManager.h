

#pragma once

#include "CinemachineCameraBlendStack.h"
#include "CinemachineBlendManager.generated.h"

UCLASS(BlueprintType, ClassGroup = (Cinemachine))
class UCinemachineBlendManager : public UCinemachineCameraBlendStack
{
	GENERATED_BODY()

public:
	ICinemachineCameraInterface* GetActiveVirtualCamera() const;

	UCinemachineBlend* GetActiveBlend() const;

	void SetActiveBlend(UCinemachineBlend* Blend);

	bool IsBlending() const;

	bool IsLiveInBlend(ICinemachineCameraInterface* ICamera) const;

	bool IsLive(ICinemachineCameraInterface* ICamera) const;

	void ComputeCurrentBlend();

	void RefreshCurrentCameraState(FVector Up, float DeltaTime);

	ICinemachineCameraInterface* ProcessActiveCamera(ICinemachineMixerInterface* Mixer, FVector Up, float DeltaTime);

protected:
	virtual void OnInitailize() override;

public:
	UPROPERTY()
	UCinemachineBlend* CurrentLiveCameras;

private:
	UPROPERTY()
	UCinemachineBlend* PreviousLiveCameras;

	UPROPERTY()
	TArray<UObject*> CameraCache;
};
