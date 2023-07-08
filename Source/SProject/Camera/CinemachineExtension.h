
#pragma once

#include "CoreMinimal.h"
#include "CinemachineCameraState.h"
#include "CinemachineExtension.generated.h"

class UCinemachineVirtualCameraBaseComponent;
enum class ECVStage : uint8;

/**
 * 가상 카메라에 대한 확장 기능을 정의하는 추상 클래스
*/
UCLASS(Abstract, ClassGroup = (Cineamchine))
class UCinemachineExtension : public UObject
{
	GENERATED_BODY()

public:
	void Init(UCinemachineVirtualCameraBaseComponent* OwnerVirtualCamera);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
		bool IsInitialized() const
	{
		return bIsInitialized;
	}

	virtual void PrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty
	}

	virtual void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime);

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

	virtual void ConnectToVCamera(bool bConnect);
protected:
	virtual void OnInitialize() { /* Needs Overriding */ }

	virtual void PostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty;
	}

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

public:
	UPROPERTY(BlueprintReadOnly, Category = "Cinemachine|Extension")
	UCinemachineVirtualCameraBaseComponent* Owner;

private:
	TMap<UCinemachineVirtualCameraBaseComponent*, UObject*> ExtraState;

private:
	bool bIsInitialized = false;
};
