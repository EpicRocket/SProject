

#include "CinemachineInputAxis.h"
#include "CinemachineCoreSubSystem.h"
#include "Shared/Damper.h"

void FCinemachineInputAxis::Reset(UWorld* World)
{
	CurrentSpeed = 0.0F;
	CancelRecentering(World);
	if (Recentering.bEnabled && (Restriction & static_cast<int32>(ECVRestrictionFlag::NoRecentering)) == 0)
	{
		Value = GetClampValue(Center);
	}
}

bool FCinemachineInputAxis::TrackValueChange(UWorld* World)
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(World);
	if (!IsValid(Subsystem))
	{
		return false;
	}

	float ClampValue = GetClampedValue();
	if (!FMath::IsNearlyEqual(ClampValue, RecenteringState.LastValue))
	{
		RecenteringState.LastValueChangeTime = Subsystem->GetUnpausedTimeSeconds();
		RecenteringState.LastValue = ClampValue;
		return true;
	}

	return false;
}

void FCinemachineInputAxis::UpdateRecentering(UWorld* World, float DeltaTime, bool bForceCancel)
{
	UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(World);
	if (!IsValid(Subsystem))
	{
		return;
	}

	if ((Restriction & (static_cast<int32>(ECVRestrictionFlag::NoRecentering) | static_cast<int32>(ECVRestrictionFlag::Momentary))) != 0)
	{
		return;
	}

	if (bForceCancel)
	{
		CancelRecentering(World);
		return;
	}
	if (RecenteringState.bForceRecenter || (Recentering.bEnabled && DeltaTime < 0.0F))
	{
		Value = Center;
		CancelRecentering(World);
	}
	else if (RecenteringState.bForceRecenter || (Recentering.bEnabled && (Subsystem->GetUnpausedTimeSeconds() - RecenteringState.LastValueChangeTime) >= Recentering.Wait))
	{
		float ClampValue = GetClampedValue();
		float Distance = FMath::Abs(Center - ClampValue);
		if (Distance < UE_KINDA_SMALL_NUMBER || Recentering.Time < UE_KINDA_SMALL_NUMBER)
		{
			ClampValue = Center;
			RecenteringState.RecenteringVelocity = 0.0F;
		}
		else
		{
			float D = Range.Y - Range.X;
			if (bWrap && Distance > D * 0.5F)
			{
				ClampValue += FMath::Sign(Center - ClampValue) * D;
			}

			ClampValue = UDamper::SmoothDamp(ClampValue, Center, RecenteringState.RecenteringVelocity, Recentering.Time * 0.5F, 9999, DeltaTime);
		}
		Value = RecenteringState.LastValue = GetClampValue(ClampValue);

		if (FMath::Abs(Value - Center) < UE_KINDA_SMALL_NUMBER)
		{
			RecenteringState.bForceRecenter = false;
		}
	}
}

void FCinemachineInputAxis::CancelRecentering(UWorld* World)
{
	if (UCinemachineCoreSubSystem* Subsystem = UWorld::GetSubsystem<UCinemachineCoreSubSystem>(World))
	{
		RecenteringState.LastValueChangeTime = Subsystem->GetUnpausedTimeSeconds();
	}
	else
	{
		RecenteringState.LastValueChangeTime = 0.0F;
	}

	RecenteringState.LastValue = GetClampedValue();
	RecenteringState.RecenteringVelocity = 0.0F;
	RecenteringState.bForceRecenter = false;
}

void FCinemachineInputAxis::ProcessInput(float InputValue, float DeltaTime)
{
	float DampTime = FMath::Abs(InputValue) < FMath::Abs(CurrentSpeed) ? DecelTime : AccelTime;
	if ((Restriction & static_cast<int32>(ECVRestrictionFlag::Momentary)) == 0)
	{
		if(DeltaTime < 0.0f)
		{
			CurrentSpeed = 0.0F;
		}
		else
		{
			CurrentSpeed += UDamper::Damp(InputValue - CurrentSpeed, DampTime, DeltaTime);

			if (!bWrap && DecelTime > UE_KINDA_SMALL_NUMBER && FMath::Abs(CurrentSpeed) > UE_KINDA_SMALL_NUMBER)
			{
				const float ClampValue = GetClampedValue();
				float Delta = CurrentSpeed > 0.0F ? Range.Y - ClampValue : ClampValue - Range.X;
				float MaxSpeed = 0.1F + 4 * Delta / DecelTime;
				if (FMath::Abs(CurrentSpeed) > FMath::Abs(MaxSpeed))
				{
					CurrentSpeed = MaxSpeed * FMath::Sign(CurrentSpeed);
				}
			}
			Value = GetClampValue(Value + CurrentSpeed * DeltaTime);
		}
	}
	else
	{
		if (DeltaTime < 0.0F)
		{
			Value = Center;
		}
		else
		{
			float DesiredValue = GetClampValue(InputValue + Center);
			Value += UDamper::Damp(DesiredValue - Value, DampTime, DeltaTime);
		}
	}
}
