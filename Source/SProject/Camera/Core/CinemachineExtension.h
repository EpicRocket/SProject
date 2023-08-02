

#pragma once

#include "CoreMinimal.h"
#include "CinemachineCameraState.h"
#include "CinemachineExtension.generated.h"

class ICinemachineCameraInterface;
class UCinemachineVirtualCameraBaseComponent;
enum class ECVStageType : uint8;

UCLASS()
class UCVExtraStateBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UCinemachineVirtualCameraBaseComponent* VCamera;
};

/**
 * 가상 카메라에 대한 확장 기능을 정의하는 추상 클래스
*/
UCLASS(Abstract, ClassGroup = (Cineamchine))
class UCinemachineExtension : public UObject
{
	GENERATED_BODY()

public:
	void Init(UCinemachineVirtualCameraBaseComponent* InOwner);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

	virtual void PrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime) { /*Needs Overriding*/ }

	virtual void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStageType Stage, FCinemachineCameraState& State, float DeltaTime);

	virtual bool OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime) { return false;/*Needs Overriding*/ }

	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) { /*Needs Overriding*/ }

	virtual float GetMaxDampTime() { return 0.0F;/*Needs Overriding*/ }

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void SetEnable(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsEnabled() const
	{
		return bExtensionEnable;
	}

protected:
	virtual void OnInitialize() { /* Needs Overriding */ }

	virtual void OnEnable();

	virtual void OnDisable();

	virtual void PostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStageType Stage, FCinemachineCameraState& State, float DeltaTime) { /*Needs Overriding*/ }

	template <typename T, typename = typename std::enable_if<std::is_base_of<UCVExtraStateBase, T>::value>::type>
	T GetExtraState(UCinemachineVirtualCameraBaseComponent* VCamera)
	{
		if (nullptr == VCamera)
		{
			return nullptr;
		}
		if (ExtraState.Contains(VCamera))
		{
			return Cast<T>(ExtraState[VCamera]);
		}
		return nullptr;
	}

	template <typename T, typename = typename std::enable_if<std::is_base_of<UCVExtraStateBase, T>::value>::type>
	TArray<T> GetAllExtraState()
	{
		TArray<T*> Result;
		for (auto& [VCamera, State] : ExtraState)
		{
			T* CastedState = Cast<T>(State);
			if (nullptr != CastedState)
			{
				Result.Emplace(CastedState);
			}
		}
		return Result;
	}

public:
	UPROPERTY(BlueprintReadOnly, Category = Cinemachine)
	UCinemachineVirtualCameraBaseComponent* Owner;

private:
	UPROPERTY()
	TMap<UCinemachineVirtualCameraBaseComponent*, UCVExtraStateBase*> ExtraState;

private:
	bool bIsInitialized = false;

	bool bExtensionEnable = false;
};
