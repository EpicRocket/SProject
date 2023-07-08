

#include "CinemachineOrbitalTransposer.h"
//! Engine Include
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
//! Module Include
#include "Camera/CinemachineCoreSubSystem.h"
#include "Camera/CinemachineVirtualCameraBaseComponent.h"
#include "Camera/CinemachineInputAxisProviderInterface.h"
#include "Shared/VectorExtension.h"

//! FHeadingTracker

struct FHeadingTracker
{
private:
	struct FItem
	{
		FVector Velocity = FVector::ZeroVector;
		float Weight = 0.0F;
		float Time = 0.0F;

		FItem()
		{
		}

		FItem(FVector InVelocity, float InWeight, float InTime)
			: Velocity(InVelocity)
			, Weight(InWeight)
			, Time(InTime)
		{
		}
	};

public:
	FHeadingTracker(int32 VelocityFilterStrength, int32 FilterSize)
	{
		for (int32 i = 0; i < FilterSize; ++i)
		{
			History.Emplace(FHeadingTracker::FItem());
		}
		DecayExponent = -FMath::Log2(2.0F) / VelocityFilterStrength;
	}

	int32 FilterSize() const
	{
		return History.Num();
	}

	void Add(FVector Velocity, float GameTime)
	{
		if (History.IsEmpty())
		{
			LastGoodHeading = Velocity;
			return;
		}
		float Weight = Velocity.SizeSquared();
		if (Weight > UE_KINDA_SMALL_NUMBER)
		{
			FHeadingTracker::FItem Item(Velocity, Weight, GameTime);
			if (Count == FilterSize())
			{
				PopBottom(GameTime);
			}
			++Count;
			if (!History.IsValidIndex(Top))
			{
				Top = 0;
			}
			History[Top++] = Item;

			WeightSum *= GetDecay(Item.Time - WeightTime);
			WeightTime = Item.Time;
			WeightSum += Weight;
			HeadingSum += Item.Velocity;
		}
	}

	void DecayHistory(float GameTime)
	{
		float Decay = GetDecay(GameTime - WeightTime);
		WeightSum *= Decay;
		WeightTime = GameTime;
		if (WeightSum < UE_KINDA_SMALL_NUMBER)
		{
			ClearHistory();
		}
		else
		{
			HeadingSum *= Decay;
		}
	}

	FVector GetReliableHeading()
	{
		if (WeightSum > KINDA_SMALL_NUMBER && (Count == FilterSize() || LastGoodHeading.IsNearlyZero()))
		{
			FVector Half = HeadingSum / WeightSum;
			if (!Half.IsNearlyZero())
			{
				LastGoodHeading = Half.GetSafeNormal();
			}
		}
		return LastGoodHeading;
	}

private:
	void ClearHistory()
	{
		Top = Bottom = Count = 0;
		WeightSum = 0;
		HeadingSum = FVector::ZeroVector;
	}

	void PopBottom(float GameTime)
	{
		if (Count--)
		{
			FHeadingTracker::FItem& Item = History[Bottom];
			if (++Bottom == FilterSize())
			{
				Bottom = 0;
			}

			float Decay = GetDecay(GameTime - Item.Time);
			WeightSum -= Item.Weight * Decay;
			HeadingSum -= Item.Velocity * Decay;
			if (WeightSum <= UE_KINDA_SMALL_NUMBER || Count == 0)
			{
				ClearHistory();
			}
		}
	}

	float GetDecay(float Time) const
	{
		return FMath::Exp(Time * DecayExponent);
	}

private:
	TArray<FHeadingTracker::FItem> History;
	int32 Top = 0;
	int32 Bottom = 0;
	int32 Count = 0;

	FVector HeadingSum = FVector::ZeroVector;
	float WeightSum = 0.0F;
	float WeightTime = 0.0F;

	FVector LastGoodHeading = FVector::ZeroVector;

	float DecayExponent;
};

//! UCinemachineOrbitalTransposer

UCinemachineOrbitalTransposer::UCinemachineOrbitalTransposer()
{
	HeadingUpdater = [this](UCinemachineOrbitalTransposer* OrbitalTransposer, float DeltaTime, FVector Up) -> float
	{
		return InternalUpdateHeading(OrbitalTransposer, DeltaTime, Up);
	};
}

#if WITH_EDITOR
void UCinemachineOrbitalTransposer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	OrbitalTransposerData.XAxis.Validate();
	OrbitalTransposerData.RecenterToTargetHeading.Validate();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UCinemachineOrbitalTransposer::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	Super::ForceCameraLocation(Location, Rotation);
	LastCameraLocation = Location;
	if (IsValid(Owner))
	{
		OrbitalTransposerData.XAxis.Value = GetAxisClosestValue(Location, Owner->GetState().ReferenceUp);
	}
}

bool UCinemachineOrbitalTransposer::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters)
{
	OrbitalTransposerData.RecenterToTargetHeading.DoRecentering(this, OrbitalTransposerData.XAxis, -1.0F, 0.0F);
	OrbitalTransposerData.RecenterToTargetHeading.CancelRecentering(this);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	if (FromCamera == nullptr)
	{
		return false;
	}

	bool bIsLiveInBlend = World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLiveInBlend(Cast<ICinemachineCameraInterface>(Owner));
	if (TransposerData.BindingMode != ECVBindingMode::SimpleFollowWithWorldUp && TransitionParameters.bInheritLocation && !bIsLiveInBlend)
	{
		OrbitalTransposerData.XAxis.Value = GetAxisClosestValue(FromCamera->GetState().RawLocation, WorldUp);
		return true;
	}

	return false;
}

void UCinemachineOrbitalTransposer::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
{
	InitPrevFrameStateInfo(State, DeltaTime);

	USceneComponent* FollowTarget = GetFollowTarget();
	if (FollowTarget != PreviouseTarget)
	{
		PreviouseTarget = FollowTarget;
		TargetMovementComponent = nullptr;
		if (IsValid(PreviouseTarget))
		{
			AActor* TargetOwner = PreviouseTarget->GetOwner();
			if (IsValid(TargetOwner) && TargetOwner->GetRootComponent() == PreviouseTarget)
			{
				TargetMovementComponent = TargetOwner->FindComponentByClass<UMovementComponent>();
			}
		}
		LastTargetLocation = !IsValid(PreviouseTarget) ? FVector::ZeroVector : PreviouseTarget->GetComponentLocation();
		HeadingTracker.Reset();
	}

	LastHeading = HeadingUpdater ? HeadingUpdater(this, DeltaTime, State.ReferenceUp) : 0.0F;
	float LastHeadingTemp = LastHeading;

	if (!IsValid(FollowTarget))
	{
		return;
	}

	if (TransposerData.BindingMode != ECVBindingMode::SimpleFollowWithWorldUp)
	{
		LastHeadingTemp += OrbitalTransposerData.Heading.Bias;
	}
	FQuat HeadingRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(LastHeadingTemp));

	FVector RawOffset = GetEffectiveOffset();
	FVector Offset = HeadingRotation * RawOffset;

	FVector Location = FVector::ZeroVector;
	FRotator Orientation = FRotator::ZeroRotator;
	TrackTarget(DeltaTime, State.ReferenceUp, Offset, Location, Orientation);

	Offset = Orientation.Quaternion() * Offset;
	State.ReferenceUp = Orientation.Quaternion() * FVector::UpVector;

	FVector TargetLocation = GetFollowTargetLocation();
	Location += GetOffsetForMinimumTargetDistance(Location, Offset, State.RawOrientation.Quaternion() * FVector::ForwardVector, State.ReferenceUp, TargetLocation);
	State.RawLocation = Location + Offset;

	if (DeltaTime >= 0.0F && IsValid(Owner))
	{
		if (Owner->GetPreviousStateIsValid())
		{
			FVector LookAtLocation = TargetLocation;
			if (IsValid(GetLookAtTarget()))
			{
				LookAtLocation = GetLookAtTargetLocation();
			}
			FVector Dir0 = LastCameraLocation - LookAtLocation;
			FVector Dir1 = State.RawLocation - LookAtLocation;
			if (Dir0.SizeSquared() > 0.01F && Dir1.SizeSquared() > 0.01F)
			{
				State.LocationDampingBypass = UVectorExtension::SafeFromToRotation(Dir0, Dir1, State.ReferenceUp).Euler();
			}
		}
	}

	LastTargetLocation = TargetLocation;
	LastCameraLocation = State.RawLocation;
}

FVector UCinemachineOrbitalTransposer::GetTargetCameraLocation(FVector WorldUp)
{
	if (!IsValid(GetFollowTarget()))
	{
		return FVector::ZeroVector;
	}
	float HeadingValue = LastHeading;
	if (TransposerData.BindingMode != ECVBindingMode::SimpleFollowWithWorldUp)
	{
		HeadingValue += OrbitalTransposerData.Heading.Bias;
	}
	FQuat Orientation = FQuat(FVector::UpVector, FMath::DegreesToRadians(HeadingValue));
	Orientation = GetReferenceOrientation(WorldUp).Quaternion() * Orientation;
	FVector Location = Orientation * GetEffectiveOffset();
	Location += LastTargetLocation;
	return Location;
}

float UCinemachineOrbitalTransposer::UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis)
{
	return UpdateHeading(DeltaTime, Up, OutAxis, OrbitalTransposerData.RecenterToTargetHeading, true);
}

float UCinemachineOrbitalTransposer::UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis, FCinemachineAxisStateRecentering& OutRecentering, bool bIsLive)
{
	if (!IsValid(Owner))
	{
		OutAxis.Reset();
		OutRecentering.CancelRecentering(this);
		return OutAxis.Value;
	}

	if (TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		OutAxis.MinValue = -180.0F;
		OutAxis.MaxValue = 180.0F;
	}

	if (DeltaTime < 0.0F || !Owner->GetPreviousStateIsValid() || !bIsLive)
	{
		OutAxis.Reset();
		OutRecentering.CancelRecentering(this);
	}
	else if (OutAxis.Update(this, DeltaTime))
	{
		OutRecentering.CancelRecentering(this);
	}

	if (TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		float FinalHeading = OutAxis.Value;
		OutAxis.Value = 0.0f;
		return FinalHeading;
	}

	float TargetHeading = GetTargetHeading(OutAxis.Value, GetReferenceOrientation(Up));
	OutRecentering.DoRecentering(this, OutAxis, DeltaTime, TargetHeading);
	return OutAxis.Value;
}

void UCinemachineOrbitalTransposer::UpdateInputAxisProvider()
{
	OrbitalTransposerData.XAxis.SetInputAxisProvider(EAxis::X, nullptr);
	if (!bHeadingIsSlave && IsValid(Owner))
	{
		OrbitalTransposerData.XAxis.SetInputAxisProvider(EAxis::X, Owner->GetInputAxisProvider());
	}
}

float UCinemachineOrbitalTransposer::GetAxisClosestValue(FVector CameraLocation, FVector Up)
{
	FQuat Orientation = GetReferenceOrientation(Up).Quaternion();
	FVector Forward = FVector::VectorPlaneProject(Orientation * FVector::ForwardVector, Up);
	if (!Forward.IsNearlyZero() && IsValid(GetFollowTarget()))
	{
		float HeadingValue = 0.0F;
		if (TransposerData.BindingMode != ECVBindingMode::SimpleFollowWithWorldUp)
		{
			HeadingValue += OrbitalTransposerData.Heading.Bias;
		}
		Orientation *= FQuat(Up, FMath::DegreesToRadians(HeadingValue));
		FVector TargetLocation = GetFollowTargetLocation();
		FVector Location = TargetLocation + Orientation * GetEffectiveOffset();

		FVector V1 = FVector::VectorPlaneProject(Location - TargetLocation, Up);
		FVector V2 = FVector::VectorPlaneProject(CameraLocation - TargetLocation, Up);
		return UVectorExtension::SignedAngle(V1, V2, Up);
	}
	return LastHeading;
}

float UCinemachineOrbitalTransposer::InternalUpdateHeading(UCinemachineOrbitalTransposer* Orbital, float DeltaTime, FVector Up)
{
	if (!IsValid(Orbital))
	{
		return 0.0F;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return 0.0F;
	}

	return  Orbital->UpdateHeading(DeltaTime, Up, Orbital->OrbitalTransposerData.XAxis, Orbital->OrbitalTransposerData.RecenterToTargetHeading, World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLive(Cast<ICinemachineCameraInterface>(Orbital->Owner)));
}

float UCinemachineOrbitalTransposer::GetTargetHeading(float CurrentHeading, FRotator TargetOrientation)
{
	if (TransposerData.BindingMode == ECVBindingMode::SimpleFollowWithWorldUp)
	{
		return 0.0F;
	}

	if (!IsValid(GetFollowTarget()))
	{
		return CurrentHeading;
	}

	ECVOrbitalTransposerHeadingDef HeadingDefinition = OrbitalTransposerData.Heading.Definition;
	if (HeadingDefinition == ECVOrbitalTransposerHeadingDef::Velocity && !IsValid(TargetMovementComponent))
	{
		HeadingDefinition = ECVOrbitalTransposerHeadingDef::LocationDelta;
	}

	FVector Velocity = FVector::ZeroVector;
	switch (HeadingDefinition)
	{
	case ECVOrbitalTransposerHeadingDef::LocationDelta:
		Velocity = GetFollowTargetLocation() - LastTargetLocation;
		break;
	case ECVOrbitalTransposerHeadingDef::Velocity:
		Velocity = TargetMovementComponent->Velocity;
		break;
	case ECVOrbitalTransposerHeadingDef::TargetForward:
		Velocity = GetFollowTargetRotation().Quaternion() * FVector::ForwardVector;
		break;

	case ECVOrbitalTransposerHeadingDef::WorldForward:	// falls through
	default:
		return 0.0F;
	}

	FVector Up = TargetOrientation.Quaternion() * FVector::UpVector;
	Velocity = FVector::VectorPlaneProject(Velocity, Up);
	if (HeadingDefinition != ECVOrbitalTransposerHeadingDef::TargetForward)
	{
		int32 FilterSize = OrbitalTransposerData.Heading.VelocityFilterStrength * 5;
		if (!HeadingTracker.IsValid() || HeadingTracker->FilterSize() != FilterSize)
		{
			HeadingTracker.Reset();
			HeadingTracker = MakeShared<FHeadingTracker>(OrbitalTransposerData.Heading.VelocityFilterStrength, FilterSize);
		}
		if (HeadingTracker.IsValid())
		{
			float GameTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);
			HeadingTracker->DecayHistory(GameTime);
			if (!Velocity.IsNearlyZero())
			{
				HeadingTracker->Add(Velocity, GameTime);
			}
			Velocity = HeadingTracker->GetReliableHeading();
		}
	}
	if (!Velocity.IsNearlyZero())
	{
		return UVectorExtension::SignedAngle(TargetOrientation.Quaternion() * FVector::ForwardVector, Velocity, Up);
	}

	return CurrentHeading;
}
