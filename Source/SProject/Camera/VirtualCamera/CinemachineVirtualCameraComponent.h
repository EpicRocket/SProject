

#pragma once

#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineBaseStage.h"
#include "CinemachineVirtualCameraComponent.generated.h"

enum class ECVStage : uint8;

/**
 * 일반적으로 사용 되는 가상 카메라 컴포넌트 입니다.
 * Follow 타겟에 대한 기능을 사용하기 위해서는 UCinemachineTransposer를 자식으로 가지고 있어야 합니다.
 * LookAt 타겟에 대한 기능을 사용하기 위해서는 UCinemachineComposer를 자식으로 가지고 있어야 합니다.
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineVirtualCameraComponent : public UCinemachineVirtualCameraBaseComponent
{
	GENERATED_BODY()

	friend class UCinemachineFreeLookComponent;

public:
	UCinemachineVirtualCameraComponent();

#if WITH_EDITOR
	//~ Begin USceneComponent Interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	//~ End USceneComponent Interface.
#endif

	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual float GetMaxDampTime() override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime) override;
	virtual bool RequiresUserInput() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

	UCinemachineBaseStage* GetCinemachineStage(ECVStage Stage);

	template <typename T, typename = typename std::enable_if<std::is_base_of<UCinemachineBaseStage, T>::value>::type>
	T* GetCinemachineStage()
	{
		for (UCinemachineBaseStage* Component : StagePipeline)
		{
			if (Component->IsA<T>())
			{
				return Cast<T>(Component);
			}
		}
		return nullptr;
	}

	UCinemachineBaseStage* AddCinemachineComponent(TSubclassOf<UCinemachineBaseStage> CreateComponent, FName ComponentName, UCinemachineBaseStage* Template);

	void SetStateRawLocation(FVector Location)
	{
		VCameraState.RawLocation = Location;
	}

	void InvalidateStagePipeline()
	{
		StagePipeline.Empty();
	}

protected:
	//~ Begin UCinemachineVirtualCameraBaseComponent Interface
	virtual void OnInitailize_Implementation() override;
	virtual void OnEnable() override;
	//~ End UCinemachineVirtualCameraBaseComponent Interface

private:
	FCinemachineCameraState CalculateNewState(FVector WorldUp, float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineTransitionParameters TransitionParameters;

private:
	UPROPERTY(VisibleAnywhere, Transient, Category = Cinemachine, Transient, meta = (AllowPrivateAccess = true, ShowInnerProperties))
	TArray<UCinemachineBaseStage*> StagePipeline;

	UPROPERTY(Transient)
	USceneComponent* DefaultCachedLookAtTarget = nullptr;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* DefaultCachedLookAtTargetVCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "AimStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> AimStageClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "BodyStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> BodyStageClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Spawn", meta = (AllowPrivateAccess = "true", ForceRebuildProperty = "NoiseStageTemplate"))
	TSubclassOf<UCinemachineBaseStage> NoiseStageClass;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> AimStageTemplate;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> BodyStageTemplate;

	UPROPERTY(VisibleDefaultsOnly, DuplicateTransient, Category = "Cinemachine|Spawn", meta = (ShowInnerProperties))
	TObjectPtr<UCinemachineBaseStage> NoiseStageTemplate;
};
