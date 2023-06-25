

#pragma once

#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineBaseComponent.h"
#include "CinemachineVirtualCameraComponent.generated.h"

enum class ECinemachineStage : uint8;

/**
 * 일반적으로 사용 되는 가상 카메라 컴포넌트 입니다.
 * Follow 타겟에 대한 기능을 사용하기 위해서는 UCinemachineTransposerComponent를 자식으로 가지고 있어야 합니다.
 * LookAt 타겟에 대한 기능을 사용하기 위해서는 UCinemachineComposerComponent를 자식으로 가지고 있어야 합니다.
*/
UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineVirtualCameraComponent : public UCinemachineVirtualCameraBaseComponent
{
	GENERATED_BODY()

public:
	UCinemachineVirtualCameraComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual float GetMaxDampTime() override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector IocationDelta) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime) override;
	virtual bool RequiresUserInput() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

	UCinemachineBaseComponent* GetCinemachineComponent(ECinemachineStage Stage);

	template <typename T, typename = typename std::enable_if<std::is_base_of<UCinemachineBaseComponent, T>::value>::type>
	T* GetCinemachineComponent()
	{
		for (UCinemachineBaseComponent* Component : ComponentPipeline)
		{
			if (Component->IsA<T>())
			{
				return Cast<T>(Component);
			}
		}
		return nullptr;
	}

	UCinemachineBaseComponent* AddCinemachineComponent(TSubclassOf<UCinemachineBaseComponent> CreateComponent, FName ComponentName);

	void SetStateRawLocation(FVector Location)
	{
		VCameraState.RawLocation = Location;
	}

	void InvalidateComponentPipeline()
	{
		ComponentPipeline.Empty();
	}

	void UpdateComponentPipeline();

protected:
	virtual void OnEnable() override;

private:
	FCinemachineCameraState CalculateNewState(FVector WorldUp, float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineTransitionParameters TransitionParameters;

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = Cinemachine, Transient, meta = (AllowPrivateAccess = true))
	TArray<UCinemachineBaseComponent*> ComponentPipeline;

	UPROPERTY(Transient)
	USceneComponent* DefaultCachedLookAtTarget;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* DefaultCachedLookAtTargetVCamera;
};
