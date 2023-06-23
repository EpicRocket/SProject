

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Curves/CurveFloat.h"
#include "CinemachineCameraState.h"
#include "CinemachineCameraInterface.h"
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

	FString GetDescription() const;

	bool Uses(ICinemachineCameraInterface* ICamera);

	void UpdateCameraState(FVector WorldUp, float DeltaTime);

	FCinemachineCameraState GetState() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	UObject* CameraA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	UObject* CameraB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	FRuntimeFloatCurve BlendCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	float TimeInBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	float Duration;
};

/**
 * 두 가상 카메라의 블렌딩을 정의하는 구조체 (주로 커브를 정의 합니다.)
*/
USTRUCT(BlueprintType)
struct FCinemachineBlendDefinition
{
	GENERATED_USTRUCT_BODY()

public:
	FCinemachineBlendDefinition();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
	FRuntimeFloatCurve Curve;
};

/**
 * 가상 카메라 블렌딩을 위한 임시 구조체
*/
UCLASS(Blueprintable, ClassGroup = (Cinemachine))
class UStaticPointVirtualCamera : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	virtual FString GetCameraName() const
	{
		return FString("None");
	}

	virtual FString GetDescription() const
	{
		return FString("");
	}

	virtual int32 GetPriority() const
	{
		return Priority;
	}

	virtual void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

	virtual USceneComponent* GetLookAt() const
	{
		return LookAt;
	}

	virtual void SetLookAt(USceneComponent* InLookAt)
	{
		LookAt = InLookAt;
	}

	virtual USceneComponent* GetFollow() const
	{
		return Follow;
	}

	virtual void SetFollow(USceneComponent* InFollow)
	{
		Follow = InFollow;
	}

	virtual FCinemachineCameraState GetState() const
	{
		return State;
	}

public:
	int32 Priority;

	UPROPERTY(Transient)
	USceneComponent* LookAt;

	UPROPERTY(Transient)
	USceneComponent* Follow;

	FCinemachineCameraState State;
};

/**
 * 블렌딩에 대한 결과 소스를 정의하는 구조체
*/
UCLASS(Blueprintable, ClassGroup = (Cinemachine))
class UBlendSourceVirtualCamera : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UBlendSourceVirtualCamera()
		: Blend(nullptr)
		, Priority(0)
		, LookAt(nullptr)
		, Follow(nullptr)
	{
	}

	virtual FString GetCameraName() const
	{
		return FString(TEXT("Mid-blend"));
	}

	virtual FString GetDescription() const
	{
		return IsValid(Blend) ? Blend->GetDescription() : FString(TEXT("None"));
	}

	virtual int32 GetPriority() const
	{
		return Priority;
	}

	virtual void SetPriority(int32 InPriority)
	{
		Priority = InPriority;
	}

	virtual USceneComponent* GetLookAt() const
	{
		return LookAt;
	}

	virtual void SetLookAt(USceneComponent* InLookAt)
	{
		LookAt = InLookAt;
	}

	virtual USceneComponent* GetFollow() const
	{
		return Follow;
	}

	virtual void SetFollow(USceneComponent* InFollow)
	{
		Follow = InFollow;
	}

	virtual FCinemachineCameraState GetState() const
	{
		return State;
	}

	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false) const
	{
		return IsValid(Blend) && (Cast<ICinemachineCameraInterface>(Blend->CameraA) == InCamera || Cast<ICinemachineCameraInterface>(Blend->CameraB) == InCamera);
	}

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime)
	{
		if (IsValid(Blend))
		{
			Blend->UpdateCameraState(WorldUp, DeltaTime);
			State = Blend->GetState();
		}
	}

	virtual FCinemachineCameraState CalculateNewState(float DeltaTime)
	{
		return GetState();
	}

public:
	UPROPERTY(Transient)
	UCinemachineBlend* Blend;

	int32 Priority;

	UPROPERTY(Transient)
	USceneComponent* LookAt;

	UPROPERTY(Transient)
	USceneComponent* Follow;

	FCinemachineCameraState State;
};
