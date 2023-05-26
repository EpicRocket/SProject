
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraState.h"
#include "CinemachineExtension.generated.h"

class UCinemachineVirtualCameraComponent;
enum class ECinemachineStage : uint8;

UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineExtension : public USceneComponent
{
	GENERATED_BODY()

public:
	UCinemachineExtension();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	UCinemachineVirtualCameraComponent* GetVirtualCamera();

	void EnsureStarted();

	virtual void PrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraComponent* VCamera, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty
	}

	virtual void InvokePostPipelineStageCallback(UCinemachineVirtualCameraComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime);

	virtual void OnTargetObjectWarped(AActor* Target, FVector LocationDelta)
	{
		// Empty
	}

	virtual void ForceCameraLocation(FVector Location, FQuat Rotation)
	{
		// Empty
	}

	virtual bool OnTransitionFromCamera(UCinemachineVirtualCameraComponent* FromVCamera, FVector WorldUp, float DeltaTime)
	{
		return false;
	}

	virtual float GetMaxDampTime()
	{
		return 0.0F;
	}

protected:
	virtual void ConnectToVCamera(bool bConnect);

	virtual void PostPipelineStageCallback(UCinemachineVirtualCameraComponent* VCamera, ECinemachineStage Stage, FCinemachineCameraState& State, float DeltaTime)
	{
		// Empty;
	}

    template<typename T, typename = typename std::enable_if<std::is_base_of<UObject, T>::value>::type>
    T GetExtraState(UCinemachineVirtualCameraComponent* VCamera)
    {
        if(ExtraState.Contains(VCamera))
        {
            return Cast<T>(ExtraState[VCamera]);
        }
        return nullptr;
    }

    template<typename T, typename = typename std::enable_if<std::is_base_of<UObject, T>::value>::type>
    TArray<T> GetAllExtraState()
    {
        TArray<T> Result;
        for(auto& Elem : ExtraState)
        {
            T* State = Cast<T>(Elem.Value);
            if(IsValid(State))
            {
                Result.Add(State);
            }
        }
        return Result;
    }

private:
	TMap<UCinemachineVirtualCameraComponent*, UObject*> ExtraState;
};
