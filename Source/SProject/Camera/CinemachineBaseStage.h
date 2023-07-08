

#pragma once

#include "CoreMinimal.h"
#include "CinemachineTransitionParameters.h"
#include "CinemachineBaseStage.generated.h"

class UCinemachineVirtualCameraBaseComponent;
class UCinemachineTargetGroupBaseComponent;
class ICinemachineCameraInterface;
enum class ECVStage : uint8;

/**
 * 가상 카메라에서 작동 되는 연산을 정의하는 가상 클래스
 */
UCLASS(Abstract, ClassGroup = (Cinemachine))
class UCinemachineBaseStage : public UObject
{
	GENERATED_BODY()

public:
	UCinemachineBaseStage();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	USceneComponent* GetFollowTarget();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	USceneComponent* GetLookAtTarget();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	UCinemachineTargetGroupBaseComponent* GetFollowTargetGroup();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FVector GetFollowTargetLocation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FRotator GetFollowTargetRotation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	UCinemachineTargetGroupBaseComponent* GetLookAtTargetGroup();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FVector GetLookAtTargetLocation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FRotator GetLookAtTargetRotation();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FCinemachineCameraState GetVirtualCameraState();

	void Init(UCinemachineVirtualCameraBaseComponent* OwnerVirtualCamera);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

	/**RefereceLookAt을 오프셋하는 등의 작업을 수행합니다. */
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& State, float DeltaTime);

	/** 컴포넌트가 어느 부분인지 반환 합니다. (Body:이동, Aim:회전)*/
	virtual ECVStage GetStage() const;

	/** Aim 갱신 후 Body를 갱신할 것인지 여부를 확인합니다. */
	virtual bool BodyAppliesAfterAim() const;

	/** CV카메라의 상태를 변경합니다. 이 상태는 나중에 카메라에 적용 됩니다. */
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime);

	/** 컴포넌트가 부착된 CV카메라가 라이브 상태가 되었다는 것을 알림니다. */
	virtual bool OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters);

	/** CV카메라를 지정된 위치와 방향으로 강제 설정 합니다. */
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation);

	/** 컴포넌트에 필요한 최대 댐핑 시간을 반환 합니다. */
	virtual float GetMaxDampTime() const;

	/** 사용자 입력이 필요한 컴포넌트는 이를 구현하고 true를 반환해야 합니다. */
	virtual bool RequiresUserInput() const;

protected:
	virtual void OnInitialize() { /* Needs Overriding */ }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	UCinemachineVirtualCameraBaseComponent* Owner = nullptr;

private:
	bool bIsInitialized = false;
};
