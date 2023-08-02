

#pragma once

#include "CoreMinimal.h"
#include "CinemachineCameraState.h"
#include "CinemachineBaseStage.generated.h"

class UCinemachineVirtualCameraBaseComponent;
class UCinemachineTargetGroupComponent;
class ICinemachineCameraInterface;

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
	USceneComponent* GetFollowTarget() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	USceneComponent* GetLookAtTarget() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	UCinemachineTargetGroupComponent* GetFollowTargetGroup() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FVector GetFollowTargetLocation() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FRotator GetFollowTargetRotation() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	UCinemachineTargetGroupComponent* GetLookAtTargetGroup() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FVector GetLookAtTargetLocation() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FRotator GetLookAtTargetRotation() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Stage")
	FCinemachineCameraState GetVirtualCameraState() const;

	void Init(UCinemachineVirtualCameraBaseComponent* OwnerVirtualCamera);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

	virtual void Validate() {}

	virtual bool IsStageValid() const { /*Needs Overrding*/ return false; }

	/**RefereceLookAt을 오프셋하는 등의 작업을 수행합니다. */
	virtual void PrePipelineMutateCameraState(FCinemachineCameraState& InState, float DeltaTime) { /*Needs Overrding*/ }

	/** 컴포넌트가 어느 부분인지 반환 합니다. (Body:이동, Aim:회전)*/
	virtual ECVStageType GetStage() const { return ECVStageType::Finalize; /*Needs Overrding*/ }

	/** Aim 갱신 후 Body를 갱신할 것인지 여부를 확인합니다. */
	virtual bool BodyAppliesAfterAim() const { return false;/*Needs Overrding*/ }

	/** CV카메라의 상태를 변경합니다. 이 상태는 나중에 카메라에 적용 됩니다. */
	virtual void MutateCameraState(FCinemachineCameraState& InState, float DeltaTime) { /*Needs Overrding*/ }

	/** 컴포넌트가 부착된 CV카메라가 라이브 상태가 되었다는 것을 알림니다. */
	virtual bool OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime) { return false;/*Needs Overrding*/ }

	/** CV카메라를 지정된 위치와 방향으로 강제 설정 합니다. */
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) { /*Needs Overrding*/ }

	/** 컴포넌트에 필요한 최대 댐핑 시간을 반환 합니다. */
	virtual float GetMaxDampTime() const { return 0.0F;/*Needs Overrding*/ }

protected:
	virtual void OnInitialize() { /* Needs Overriding */ }

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UCinemachineVirtualCameraBaseComponent* Owner = nullptr;

private:
	bool bIsInitialized = false;
};
