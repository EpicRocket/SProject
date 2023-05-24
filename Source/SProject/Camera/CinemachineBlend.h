
#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "CinemachineCameraState.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineBlend.generated.h"

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

UCLASS(Blueprintable)
class UStaticPointVirtualCamera : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	virtual FString GetCameraName() const
	{
		return FString("");
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

	virtual AActor* GetLookAt()
	{
		return LookAt;
	}

	virtual void SetLookAt(AActor* InLookAt)
	{
		LookAt = InLookAt;
	}

	virtual AActor* GetFollow()
	{
		return Follow;
	}

	virtual void SetFollow(AActor* InFollow)
	{
		Follow = InFollow;
	}

	virtual FCinemachineCameraState GetState()
	{
		return State;
	}

public:
	int32 Priority;

	UPROPERTY()
	AActor* LookAt;

	UPROPERTY()
	AActor* Follow;

	FCinemachineCameraState State;
};

UCLASS(Blueprintable)
class UBlendSourceVirtualCamera : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UBlendSourceVirtualCamera()
		: Blend(nullptr), Priority(0), LookAt(nullptr), Follow(nullptr)
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

	virtual AActor* GetLookAt()
	{
		return LookAt;
	}

	virtual void SetLookAt(AActor* InLookAt)
	{
		LookAt = InLookAt;
	}

	virtual AActor* GetFollow()
	{
		return Follow;
	}

	virtual void SetFollow(AActor* InFollow)
	{
		Follow = InFollow;
	}

	virtual FCinemachineCameraState GetState()
	{
		return State;
	}

	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false)
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
	UPROPERTY()
	UCinemachineBlend* Blend;

	int32 Priority;

	UPROPERTY()
	AActor* LookAt;

	UPROPERTY()
	AActor* Follow;

	FCinemachineCameraState State;
};
