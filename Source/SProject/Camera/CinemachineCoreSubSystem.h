

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinemachineBrainComponent.h"
#include "CinemachineBlend.h"
#include "CinemachineCoreSubSystem.generated.h"

UENUM(BlueprintType)
enum class ECVStage : uint8
{
	Body,
	Aim,
	Noise,
	Finalize,
};

DECLARE_DYNAMIC_DELEGATE_RetVal_FourParams(FCinemachineBlendDefinition, FCinemachineBlendDelegate, UObject*, FromVCam, UObject*, ToVCam, FCinemachineBlendDefinition, DefaultBlend, UObject*, Owner);

class UCinemachineBrainComponent;
class UCinemachineVirtualCameraBaseComponent;
class ICinemachineCameraInterface;
class USceneComponent;

/**
 * CineamchineBrainComponent 및 CinemachineVirtualCameraBaseComponent의 전체 목록과 가상 카메라의 우선 순위를 관리 합니다.
*/
UCLASS()
class UCinemachineCoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UCinemachineCoreSubSystem();

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	int32 GetBrainCameraCount() const;

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	UCinemachineBrainComponent* GetBrainCamera(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void AddActiveBrain(UCinemachineBrainComponent* BrainComponent);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void RemoveActiveBrain(UCinemachineBrainComponent* BrainComponent);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	int32 GetVirtualCameraCount() const;

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	UCinemachineVirtualCameraBaseComponent* GetVirtualCamera(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void AddActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void RemoveActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void CameraEnabled(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void CameraDisabled(UCinemachineVirtualCameraBaseComponent* VCamera);

	void UpdateAllActiveVirtualCameras(FVector WorldUp, float DeltaTime);

	void UpdateVirtualCamera(UCinemachineVirtualCameraBaseComponent* VCamera, FVector WorldUp, float DeltaTime);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* GetUpdateTarget(UCinemachineVirtualCameraBaseComponent* VCamera);

	bool IsLive(ICinemachineCameraInterface* ICamera);

	bool IsLiveInBlend(ICinemachineCameraInterface* ICamera);

	void GenerateCameraActivationEvent(UCinemachineVirtualCameraBaseComponent* VCamera, UCinemachineVirtualCameraBaseComponent* VCameraFrom);

	void GenerateCameraCutEvent(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	UCinemachineBrainComponent* FindPotentialTargetBrain(UCinemachineVirtualCameraBaseComponent* VCamera);

public:
	UPROPERTY(BlueprintReadWrite, Category = Cinemachine)
	FCinemachineBlendDelegate OnBlendOverride;

	UPROPERTY(BlueprintAssignable, Category = CinemachineBrain)
	FCinemachineBrainEvent CameraUpdatedEvent;

	UPROPERTY(BlueprintAssignable, Category = CinemachineBrain)
	FCinemachineBrainEvent CameraCutEvent;

private:
	TArray<UCinemachineBrainComponent*> ActiveBrains;

	TArray<UCinemachineVirtualCameraBaseComponent*> ActiveVirtualCameras;

	TArray<TArray<UCinemachineVirtualCameraBaseComponent*>> AllVirtualCameras;

	UCinemachineVirtualCameraBaseComponent* RoundRobinVCameraLastFrame;

	bool bActiveCameraAreSorted;

	int32 ActivationSequence;
};
