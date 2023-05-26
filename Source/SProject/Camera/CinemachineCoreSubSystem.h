
#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinemachineBlend.h"
#include "CinemachineCoreSubSystem.generated.h"

UENUM(BlueprintType)
enum class ECinemachineStage : uint8
{
	Body,
	Aim,
	Noise,
	Finalize,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCinemachineBlendDelegate, UObject*, FromVCam, UObject*, ToVCam, FCinemachineBlendDefinition, DefaultBlend, UObject*, Owner);

class UCinemachineBrainComponent;
class UCinemachineVirtualCameraComponent;
class UCinemachineUpdateTrackerStatus;
class AActor;

UCLASS()
class SPROJECT_API UCinemachineCoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UCinemachineCoreSubSystem();

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	int32 GetBrainCameraCount() const;

	UCinemachineBrainComponent* GetBrainCamera(int32 Index) const;

	void AddActiveBrain(UCinemachineBrainComponent* BrainComponent);

	void RemoveActiveBrain(UCinemachineBrainComponent* BrainComponent);

	int32 GetVirtualCameraCount() const;

	UCinemachineVirtualCameraComponent* GetVirtualCamera(int32 Index);

	void AddActiveCamera(UCinemachineVirtualCameraComponent* VCamera);

	void RemoveActiveCamera(UCinemachineVirtualCameraComponent* VCamera);

	void CameraEnabled(UCinemachineVirtualCameraComponent* VCamera);

	void CameraDisabled(UCinemachineVirtualCameraComponent* VCamera);

	void UpdateAllActiveVirtualCameras(FVector WorldUp, float DeltaTime);

	void UpdateVirtualCamera(UCinemachineVirtualCameraComponent* VCamera, FVector WorldUp, float DeltaTime);

	AActor* GetUpdateTarget(UCinemachineVirtualCameraComponent* VCamera);

	bool IsLive(UObject* ICamera);

	bool IsLiveBlend(UObject* ICamera);

	void GenerateCameraActivationEvent(UCinemachineVirtualCameraComponent* VCamera, UCinemachineVirtualCameraComponent* VCameraFrom);

	void GenerateCameraCutEvent(UCinemachineVirtualCameraComponent* VCamera);

	UCinemachineBrainComponent* FindPotentialTargetBrain(UCinemachineVirtualCameraComponent* VCamera);

	void OnTargetObjectWarped(AActor* Target, FVector LocationDelta);

public:
	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineBlendDelegate OnBlend;

	bool bSmartUpdate;

private:
	TArray<UCinemachineBrainComponent*> ActiveBrains;

	TArray<UCinemachineVirtualCameraComponent*> ActiveVirtualCameras;

	TArray<TArray<UCinemachineVirtualCameraComponent*>> AllVirtualCameras;

	UCinemachineVirtualCameraComponent* RoundRobinVCameraLastFrame;

	bool bActiveCameraAreSorted;

	int32 ActivationSequence;
};
