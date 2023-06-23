

#pragma once

#include "Components/SceneComponent.h"
#include "CinemachineTransitionParameters.h"
#include "CinemachineBaseComponent.generated.h"

class UCinemachineVirtualCameraBaseComponent;
class UCinemachineTargetGroupBaseComponent;
class ICinemachineCameraInterface;
enum class ECinemachineStage : uint8;

/**
 * 가상 카메라에서 작동 되는 연산을 정의하는 가상 클래스
 */
UCLASS(Abstract, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCinemachineBaseComponent();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	UCinemachineVirtualCameraBaseComponent* GetCinemachineVirtualCameraBaseComponent();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	USceneComponent* GetFollowTarget();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	USceneComponent* GetLookAtTarget();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	UCinemachineTargetGroupBaseComponent* GetFollowTargetGroup();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	FVector GetFollowTargetLocation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	FRotator GetFollowTargetRotation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	UCinemachineTargetGroupBaseComponent* GetLookAtTargetGroup();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	FVector GetLookAtTargetLocation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	FRotator GetLookAtTargetRotation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Component")
	FCinemachineCameraState GetVirtualCameraState();

	/** Virtual Methods */
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime);
	virtual ECinemachineStage GetStage() const;
	virtual bool BodyAppliesAfterAim() const;
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime);
	virtual bool OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters);
	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta);
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation);
	virtual float GetMaxDampTime() const;
	virtual bool RequriesUserInput() const;

private:
	UPROPERTY(VisibleAnywhere, Category = Cinemachine, meta = (AllowPrivateAccess = true))
	UCinemachineVirtualCameraBaseComponent* VCameraOwner = nullptr;
};
