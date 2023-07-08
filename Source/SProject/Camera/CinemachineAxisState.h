

#pragma once

#include "CinemachineInputAxisProviderInterface.h"
#include "CinemachineAxisState.generated.h"

USTRUCT(BlueprintType)
struct FCinemachineAxisStateRecentering
{
	GENERATED_USTRUCT_BODY()

public:
	FCinemachineAxisStateRecentering()
	{
	}

	FCinemachineAxisStateRecentering(bool bInEnabled, float InWaitTime, float InRecenteringTime)
		: bEnabled(bInEnabled)
		, WaitTime(InWaitTime)
		, RecenteringTime(InRecenteringTime)
		, LastUpdateTime(0.0F)
		, LastAxisInputTime(0.0F)
		, RecenteringVelocity(0.0F)
	{
	}

	void Validate()
	{
		WaitTime = FMath::Max(0.0F, WaitTime);
		RecenteringTime = FMath::Max(0.0F, RecenteringTime);
	}

	void CopyStateFrom(FCinemachineAxisStateRecentering& OutRecentering)
	{
		if (!FMath::IsNearlyEqual(LastAxisInputTime, OutRecentering.LastAxisInputTime))
		{
			RecenteringTime = 0.0F;
		}
		LastAxisInputTime = OutRecentering.LastAxisInputTime;
	}

	void CancelRecentering(const UObject* WorldContextObject);

	void RecenterNow()
	{
		LastAxisInputTime = -1.0F;
	}

	void DoRecentering(const UObject* WorldContextObject, struct FCinemachineAxisState& InOutAxisState, float DeltaTime, float RecenterTarget);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecenteringTime;

	float LastUpdateTime;

	float LastAxisInputTime;

	float RecenteringVelocity;
};

USTRUCT(BlueprintType)
struct FCinemachineAxisState
{
	GENERATED_USTRUCT_BODY()

public:
	FCinemachineAxisState();

	FCinemachineAxisState(float InMinValue, float InMaxValue, bool bInWarp, bool bInRangeLocked, float InMaxSpeed, float InAccelerateTime, float InDecelerateTime);

	void Validate()
	{
		if (!bEnhancePointerPrecision)
		{
			MaxSpeed = FMath::Max(0.0F, MaxSpeed);
		}
		AccelerateTime = FMath::Max(0.0F, AccelerateTime);
		DecelerateTime = FMath::Max(0.0F, DecelerateTime);
		MaxValue = FMath::Clamp(MaxValue, MinValue, MaxValue);
	}

	void Reset()
	{
		InputAxisValue = 0.0F;
		CurrentSpeed = 0.0F;
		LastUpdateTime = 0.0F;
		LastUpdateFrame = 0.0F;
	}

	void SetInputAxisProvider(EAxis::Type InputAxis, ICinemachineInputAxisProviderInterface* InputAxisProvider)
	{
		Provider = InputAxisProvider;
		ProviderAxis = InputAxis;
	}

	bool HasInputProvider() const
	{
		return Provider && ProviderAxis != EAxis::None;
	}

	bool Update(const UObject* WorldContextObject, float DeltaTime);

	float ClampValue(float InputValue)
	{
		float Result = MaxValue - MinValue;
		if (bWarp && Result > UE_KINDA_SMALL_NUMBER)
		{
			InputValue = FMath::Fmod(InputValue - MinValue, Result);
			float Offset = (InputValue < 0.0F) ? Result : 0.0F;
			InputValue += MinValue + Offset;
		}
		return FMath::Clamp(InputValue, MinValue, MaxValue);
	}

	bool MaxSpeedUpdate(float Input, float DeltaTime);

	float GetMaxSpeed() const
	{
		float Range = MaxValue - MinValue;
		if (!bWarp && Range > UE_KINDA_SMALL_NUMBER)
		{
			float Threshold = Range * 0.1F;
			if (CurrentSpeed > 0 && (MaxValue - MinValue) < Threshold)
			{
				float Scale = (MaxValue - MinValue) / Threshold;
				return FMath::Lerp(0.0F, MaxSpeed, Scale);
			}
			else if (CurrentSpeed < 0 && (Value - MinValue) < Threshold)
			{
				float Scale = (Value - MinValue) / Threshold;
				return FMath::Lerp(0.0F, MaxSpeed, Scale);
			}
		}
		return MaxSpeed;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnhancePointerPrecision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelerateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecelerateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "0 이라면 입력이 없는 상태 입니다."))
	float InputAxisValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWarp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCinemachineAxisStateRecentering Recentering;

	float CurrentSpeed;

	float LastUpdateTime;

	uint64 LastUpdateFrame;

	bool bValueRangeLocked;

	bool bHasRecentering;

protected:
	ICinemachineInputAxisProviderInterface* Provider;

	EAxis::Type ProviderAxis;
};
