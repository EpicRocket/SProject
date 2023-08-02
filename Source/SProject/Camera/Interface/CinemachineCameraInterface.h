

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "Camera/Core/CinemachineCameraState.h"
#include "CinemachineCameraInterface.generated.h"

class ICinemachineMixerInterface;
class ICinemachineCameraInterface;

USTRUCT(BlueprintType)
struct FCVActivationEventParams
{
	GENERATED_BODY()

	FCVActivationEventParams() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ICinemachineMixerInterface> Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ICinemachineCameraInterface> OutgoingCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ICinemachineCameraInterface> IncomingCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCut = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldUp = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaTime = 0.0F;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinemachineActivationEvent, FCVActivationEventParams, Params);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineCameraInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual bool IsCameraValid() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FString GetCameraName() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FCinemachineCameraState GetState() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual TScriptInterface<ICinemachineMixerInterface> GetParentCamera()
	{
		return nullptr;
	}

	/** CV카메라 상태 갱신 */
	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) { /* Needs Overriding */ }

	/** 카메라 활성화 시 이벤트 호출 */
	virtual void OnCameraActivated(FCVActivationEventParams Params) { /* Needs Overriding */ }
};

FORCEINLINE bool IsCinemachineCameraValid(ICinemachineCameraInterface* ICamera)
{
	return IsValid(Cast<UObject>(ICamera)) && ICamera->IsCameraValid();
}

FORCEINLINE bool IsCinemachineCameraValid(TScriptInterface<ICinemachineCameraInterface> ICamera)
{
	return IsCinemachineCameraValid(Cast<ICinemachineCameraInterface>(ICamera.GetObject()));
}

FORCEINLINE TScriptInterface<ICinemachineCameraInterface> GetCinemachineCameraInterface(ICinemachineCameraInterface* ICamera)
{
	TScriptInterface<ICinemachineCameraInterface> Result = nullptr;
	if (IsCinemachineCameraValid(ICamera))
	{
		Result.SetObject(Cast<UObject>(ICamera));
		Result.SetInterface(ICamera);
	}
	return Result;
}

template<typename T = UObject>
FORCEINLINE TScriptInterface<ICinemachineCameraInterface> GetCinemachineCameraInterface(T* Object)
{
	TScriptInterface<ICinemachineCameraInterface> Result = nullptr;
	if (IsValid(Object))
	{
		Result.SetObject(Object);
		Result.SetInterface(Cast<ICinemachineCameraInterface>(Object));
	}
	return Result;
}
