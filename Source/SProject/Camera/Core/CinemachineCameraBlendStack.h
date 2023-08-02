

#pragma once

#include "CinemachineBlend.h"
#include "Camera/Interface/CinemachineCameraOverrideStackInterface.h"
#include "Camera/Interface/CinemachineMixerInterface.h"
#include "CinemachineCameraBlendStack.generated.h"

UCLASS(BlueprintType, ClassGroup = (Cinemachine))
class UCVSnapshotBlendSource : public UObject, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	// ~Begin ICinemachineCameraInterface
	virtual bool IsCameraValid() const override
	{
		return true;
	}

	virtual FString GetCameraName() const override
	{
		return SnapshotName;
	}

	virtual FCinemachineCameraState GetState() const override
	{
		return State;
	}
	// ~End ICinemachineCameraInterface

	void TakeSnapshot(ICinemachineCameraInterface* Source);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCinemachineCameraState State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SnapshotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTimeInBlend = 0.0F;
};

UCLASS(BlueprintType, ClassGroup = (Cinemachine))
class UCVBlendStackFrame : public UCinemachineNestedBlendSource
{
	GENERATED_BODY()

public:
	UCVBlendStackFrame() = default;

	void Init();

	ICinemachineCameraInterface* GetSnapshotIfAppropriate(ICinemachineCameraInterface* ICamera, float Weight);

	bool IsActive() const
	{
		return IsValid(Source);
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCinemachineBlend* Source = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCVSnapshotBlendSource* Snapshot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* SnapshotSource = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SnapshotBlendWeight = 0.0F;
};

UCLASS()
class UCinemachineCameraBlendStack : public UObject, public ICinemachineCameraOverrideStackInterface
{
	GENERATED_BODY()

public:
	UCinemachineCameraBlendStack() = default;

	// ~Begin ICinemachineCameraOverrideStackInterface
	virtual int32 SetCameraOverride(int32 OverrideId, int32 Priority, ICinemachineCameraInterface* CameraA, ICinemachineCameraInterface* CameraB, float WeightB, float DeltaTime) override;
	virtual void ReleaseCameraOverride(int32 OverrideId) override;
	// ~End ICinemachineCameraOverrideStackInterface

	void Init();

	int32 FindFrame(int32 WithId, int32 Priority);

	void SetEnable(bool bEnable);

	bool IsInitialized() const;

	void ResetRootFrame();

	void UpdateRootFrame(ICinemachineMixerInterface* Context, ICinemachineCameraInterface* ActiveCamera, FVector Up, float DeltaTime);

	void ProcessOverrideFrames(UCinemachineBlend* OutputBlend, int32 NumTopLayersToExclude);

	void SetRootBlend(UCinemachineBlend* Blend);

protected:
	virtual void OnInitailize() { /*Needs Overrding*/ }

	virtual void OnEnable();

	virtual void OnDisable();

public:
	FAlphaBlend LookupBlend;

private:
	bool bIsInitialized = false;

	bool bCacheEnable = false;

	UPROPERTY()
	TArray<UCVBlendStackFrame*> FrameStack;

	int32 NextFrameId = 1;
};

extern EAlphaBlendOption GDefaultCinemachineBlendStackAlphaOption;
extern UCurveFloat* GDefaultCinemachineBlendStackCustomCurve;
