
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraState.h"
#include "CinemachineExtension.generated.h"

class UCinemachineVirtualCameraBaseComponent;
enum class ECinemachineStage : uint8;

/**
 * 가상 카메라에 대한 확장 기능을 정의하는 추상 클래스
*/
UCLASS(Abstract, ClassGroup = (Cineamchine), meta = (BlueprintSpawnableComponent))
class UCinemachineExtension : public USceneComponent
{
	GENERATED_BODY()

public:
	UCinemachineExtension();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UActorComponent Interface

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Extension")
	UCinemachineVirtualCameraBaseComponent* GetVirtualCamera();

	virtual void PrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty
	}

	virtual void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime);

	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
	{
		// Empty
	}

	virtual void ForceCameraLocation(FVector Location, FRotator Rotation)
	{
		// Empty
	}

	virtual bool OnTransitionFromCamera(UCinemachineVirtualCameraBaseComponent* FromVCamera, FVector WorldUp, float DeltaTime)
	{
		return false;
	}

	virtual float GetMaxDampTime()
	{
		return 0.0F;
	}

	virtual bool RequiresUserInput()
	{
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Extension")
	void SetEnable(bool bEnable);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Extension")
	bool IsEnable() const
	{
		return bExtensionEnable;
	}

protected:
	virtual void ConnectToVCamera(bool bConnect);

	virtual void PostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty;
	}

	virtual void OnEnable();

	virtual void OnDisable();

	template <typename T, typename = typename std::enable_if<std::is_base_of<UObject, T>::value>::type>
	T GetExtraState(UCinemachineVirtualCameraBaseComponent* VCamera)
	{
		if (ExtraState.Contains(VCamera))
		{
			return Cast<T>(ExtraState[VCamera]);
		}
		return nullptr;
	}

	template <typename T, typename = typename std::enable_if<std::is_base_of<UObject, T>::value>::type>
	TArray<T> GetAllExtraState()
	{
		TArray<T> Result;
		for (auto& Elem : ExtraState)
		{
			T* State = Cast<T>(Elem.Value);
			if (IsValid(State))
			{
				Result.Add(State);
			}
		}
		return Result;
	}

protected:
	UPROPERTY(EditAnywhere, Category = "Cinemachine|Extension")
	bool bExtensionEnable;

private:
	TMap<UCinemachineVirtualCameraBaseComponent*, UObject*> ExtraState;

	bool bCacheExtensionEnable;
};
