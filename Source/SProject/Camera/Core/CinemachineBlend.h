

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AlphaBlend.h"
#include "CinemachineCameraState.h"
#include "Camera/Interface/CinemachineCameraInterface.h"
#include "CinemachineBlend.generated.h"

/**
 * 블렌딩 대상인 두 카메라의 정보를 가지고 있는 객체
*/
UCLASS(BlueprintType)
class UCinemachineBlend : public UObject
{
	GENERATED_BODY()

public:
	UCinemachineBlend();

	float GetBlendWeight() const;

	bool IsComplete() const;

	void CopyFrom(const UCinemachineBlend* InOther);

	//CameraB 제외한 모든 필드를 지웁니다.  이렇게 하면 블렌드의 끝 부분을 효과적으로 잘라낼 수 있습니다.
	void ClearBlend();

	bool Uses(ICinemachineCameraInterface* ICamera);

	void UpdateCameraState(FVector WorldUp, float DeltaTime);

	FCinemachineCameraState GetState() const;

	bool IsCameraValid() const
	{
		ICinemachineCameraInterface* ICameraA = Cast<ICinemachineCameraInterface>(CameraA);
		bool bIsExistCameraA = IsValid(CameraA) && ICameraA != nullptr && ICameraA->IsCameraValid();
		
		ICinemachineCameraInterface* ICameraB = Cast<ICinemachineCameraInterface>(CameraB);
		bool bIsExistCameraB = IsValid(CameraB) && ICameraB != nullptr && ICameraB->IsCameraValid();

		return bIsExistCameraA || bIsExistCameraB;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	UObject* CameraA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	UObject* CameraB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FAlphaBlend BlendCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	float TimeInBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	float Duration;
};

/**
 * 블렌딩에 대한 결과 소스를 정의하는 구조체
*/
UCLASS(BlueprintType, ClassGroup = (Cinemachine))
class UCinemachineNestedBlendSource : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UCinemachineNestedBlendSource() = default;

	virtual bool IsCameraValid() const
	{
		return IsValid(Blend) && Blend->IsCameraValid();
	}

	virtual FString GetCameraName() const
	{
		if (Name.IsEmpty() && IsValid(Blend))
		{
			if (ICinemachineCameraInterface* ICamera = Cast<ICinemachineCameraInterface>(Blend->CameraB))
			{
				Name = FString::Printf(TEXT("Mid-Blend to %s"), *ICamera->GetCameraName());
			}
		}
		return Name.IsEmpty() ? TEXT("(NULL)") : Name;
	}

	virtual FCinemachineCameraState GetState() const
	{
		return State;
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		if (IsValid(Blend))
		{
			Blend->UpdateCameraState(WorldUp, DeltaTime);
			State = Blend->GetState();
		}
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	UCinemachineBlend* Blend = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	mutable FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineCameraState State;
};
