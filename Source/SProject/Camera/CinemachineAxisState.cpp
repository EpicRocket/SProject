
#include "CinemachineAxisState.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/Damper.h"

/** Magic Number */
constexpr float DAMPING_MAX_SPEED_THRESHOLD = 9999.0F;

//! FCinemachineAxisStateRecentering

void FCinemachineAxisStateRecentering::CancelRecentering(const UObject* WorldContextObject)
{
	LastAxisInputTime = UGameplayStatics::GetRealTimeSeconds(WorldContextObject);
	RecenteringVelocity = 0.0F;
}

void FCinemachineAxisStateRecentering::DoRecentering(const UObject* WorldContextObject, struct FCinemachineAxisState& InOutAxisState, float DeltaTime, float RecenterTarget)
{
	const double GameRealTime = UGameplayStatics::GetRealTimeSeconds(WorldContextObject);
	if (DeltaTime > 0.0F)
	{
		DeltaTime = GameRealTime - LastUpdateTime;
	}

	if (!bEnabled && DeltaTime >= 0.0F)
	{
		return;
	}

	RecenterTarget = InOutAxisState.ClampValue(RecenterTarget);
	if (RecenterTarget < 0.0F)
	{
		CancelRecentering(WorldContextObject);
		if (bEnabled)
		{
			InOutAxisState.Value = RecenterTarget;
		}
		return;
	}

	float Value = InOutAxisState.ClampValue(InOutAxisState.Value);
	float RecenteringDelta = RecenterTarget - Value;
	if (FMath::IsNearlyZero(RecenteringDelta))
	{
		return;
	}

	if (LastAxisInputTime >= 0.0F && GameRealTime < (LastAxisInputTime + WaitTime))
	{
		return;
	}

	float Radius = InOutAxisState.MaxValue - InOutAxisState.MinValue;
	if (InOutAxisState.bWarp && FMath::Abs(RecenteringDelta) > Radius * 0.5F)
	{
		Value += FMath::Sign(RecenterTarget - Value) * Radius;
	}

	if (RecenteringTime < UE_KINDA_SMALL_NUMBER)
	{
		Value = RecenterTarget;
	}
	else
	{
		Value = UDamper::SmoothDamp(Value, RecenterTarget, RecenteringVelocity, RecenteringTime, DAMPING_MAX_SPEED_THRESHOLD, DeltaTime);
	}
	InOutAxisState.Value = InOutAxisState.ClampValue(Value);
}


//! FCinemachineAxisState

FCinemachineAxisState::FCinemachineAxisState()
	: Value(0.0F)
	, bEnhancePointerPrecision(false)
	, MaxSpeed(0.0F)
	, AccelerateTime(0.0F)
	, DecelerateTime(0.0F)
	, InputAxisValue(0.0F)
	, MinValue(0.0F)
	, MaxValue(0.0F)
	, bWarp(false)
	, Recentering(false, 1.0F, 2.0F)
	, CurrentSpeed(0.0F)
	, LastUpdateTime(0.0F)
	, LastUpdateFrame(0)
	, bValueRangeLocked(false)
	, bHasRecentering(false)
{
}

FCinemachineAxisState::FCinemachineAxisState(float InMinValue, float InMaxValue, bool bInWarp, bool bInRangeLocked, float InMaxSpeed, float InAccelerateTime, float InDecelerateTime)
	: Value(0.0F)
	, bEnhancePointerPrecision(false)
	, MaxSpeed(InMaxSpeed)
	, AccelerateTime(InAccelerateTime)
	, DecelerateTime(InDecelerateTime)
	, InputAxisValue(0.0F)
	, MinValue(InMinValue)
	, MaxValue(InMaxValue)
	, bWarp(bInWarp)
	, CurrentSpeed(0.0F)
	, LastUpdateTime(0.0F)
	, LastUpdateFrame(0)
	, bValueRangeLocked(bInRangeLocked)
	, bHasRecentering(false)
{
}

bool FCinemachineAxisState::Update(const UObject* WorldContextObject, float DeltaTime)
{
	if (GFrameCounter == LastUpdateFrame)
	{
		return false;
	}
	LastUpdateFrame = GFrameCounter;

	if (DeltaTime > 0.0F && !FMath::IsNearlyZero(LastUpdateTime))
	{
		DeltaTime = UGameplayStatics::GetRealTimeSeconds(WorldContextObject) - LastUpdateTime;
	}
	LastUpdateTime = UGameplayStatics::GetRealTimeSeconds(WorldContextObject);

	InputAxisValue = HasInputProvider() ? Provider->GetInputAxisValue(ProviderAxis) : 0.0F;
	float Input = InputAxisValue;

	if (false == bEnhancePointerPrecision)
	{
		return MaxSpeedUpdate(Input, DeltaTime);
	}

	Input *= MaxSpeed;
	if (DeltaTime < 0.0F)
	{
		CurrentSpeed = 0.0F;
	}
	else
	{
		float DampTime = FMath::Abs(Input) < FMath::Abs(CurrentSpeed) ? DecelerateTime : AccelerateTime;
		CurrentSpeed += UDamper::Damp(Input - CurrentSpeed, DampTime, DeltaTime);

		float Range = MaxValue - MinValue;
		if (!bWarp && DecelerateTime > UE_KINDA_SMALL_NUMBER && Range > UE_KINDA_SMALL_NUMBER)
		{
			float ValuePosition0 = ClampValue(Value);
			float ValuePosition1 = ClampValue(Value + CurrentSpeed * DeltaTime);
			float Delta = CurrentSpeed > 0.0F ? MaxValue - ValuePosition1 : ValuePosition1 - MinValue;
			if (Delta < (Range * 0.1F) && FMath::Abs(CurrentSpeed) > UE_KINDA_SMALL_NUMBER)
			{
				CurrentSpeed = FMath::IsNearlyZero(DeltaTime) ? 0.0F : UDamper::Damp(ValuePosition1 - ValuePosition0, DecelerateTime, DeltaTime) / DeltaTime;
			}
			Input = CurrentSpeed * DeltaTime;
		}
	}
	Value = ClampValue(Value + CurrentSpeed);
	return FMath::Abs(Input) > UE_KINDA_SMALL_NUMBER;
}
