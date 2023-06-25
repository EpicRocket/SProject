

#include "CinemachineOrbitalTransposerComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
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

//! UCinemachineOrbitalTransposerComponent

UCinemachineOrbitalTransposerComponent::UCinemachineOrbitalTransposerComponent()
	: Heading(ECinemachineOrbitalTransposerHeadingDefinition::TargetForward, 4, 0.0F)
	, RecenterToTargetHeading(true, 1.0F, 2.0F)
	, XAxis(-180.0F, 180.0F, true, false, 300.0F, 0.1F, 0.1F)
	, bHeadingIsSlave(false)
{
}

void UCinemachineOrbitalTransposerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HeadingUpdater)
	{
		HeadingUpdater = [this](UCinemachineOrbitalTransposerComponent* OrbitalTransposer, float DeltaTime, FVector Up) -> float
		{
			return InternalUpdateHeading(OrbitalTransposer, DeltaTime, Up);
		};
	}
}

#if WITH_EDITOR
void UCinemachineOrbitalTransposerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	XAxis.Validate();
	RecenterToTargetHeading.Validate();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UCinemachineOrbitalTransposerComponent::OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta)
{
	Super::OnTargetObjectWarped(Target, LocationDelta);
	if (Target == GetFollowTarget())
	{
		LastTargetLocation += LocationDelta;
		LastCameraLocation += LocationDelta;
	}
}

void UCinemachineOrbitalTransposerComponent::ForceCameraLocation(FVector Location, FRotator Rotation)
{
	Super::ForceCameraLocation(Location, Rotation);
	LastCameraLocation = Location;
	if (UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent())
	{
		XAxis.Value = GetAxisClosestValue(Location, VCamera->GetState().ReferenceUp);
	}
}

bool UCinemachineOrbitalTransposerComponent::OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters)
{
	RecenterToTargetHeading.DoRecentering(this, XAxis, -1.0F, 0.0F);
	RecenterToTargetHeading.CancelRecentering(this);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	if (FromCamera == nullptr)
	{
		return false;
	}

	bool bIsLiveInBlend = World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLiveInBlend(Cast<ICinemachineCameraInterface>(GetCinemachineVirtualCameraBaseComponent()));
	if (BindingMode != ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp && TransitionParameters.bInheritLocation && !bIsLiveInBlend)
	{
		XAxis.Value = GetAxisClosestValue(FromCamera->GetState().RawLocation, WorldUp);
		return true;
	}

	return false;
}

void UCinemachineOrbitalTransposerComponent::MutateCameraState(FCinemachineCameraState& State, float DeltaTime)
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

	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		LastHeadingTemp += Heading.Bias;
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

	return;
	// FIXME: 카메라 흔들리는 버그 있음 코드 수정 필요
	/*if (UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent())
	{
		if (DeltaTime >= 0.0F && VCamera->GetPreviousStateIsValid())
		{
			FVector LookAtLocation = TargetLocation;
			if (IsValid(GetLookAtTarget()))
			{
				LookAtLocation = GetLookAtTargetLocation();
			}
			FVector Direction0 = LastCameraLocation - LookAtLocation;
			FVector Direction1 = State.RawLocation - LookAtLocation;
			if (Direction0.SizeSquared() > 0.01F && Direction1.SizeSquared() > 0.01F)
			{
				State.LocationDampingBypass = UVectorExtension::SafeFromToRotation(Direction0, Direction1, State.ReferenceUp).Euler();
			}
		}
	}
	LastTargetLocation = TargetLocation;
	LastCameraLocation = State.RawLocation;*/
}

FVector UCinemachineOrbitalTransposerComponent::GetTargetCameraLocation(FVector WorldUp)
{
	if (!IsValid(GetFollowTarget()))
	{
		return FVector::ZeroVector;
	}
	float HeadingValue = LastHeading;
	if (BindingMode != ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		HeadingValue += Heading.Bias;
	}
	FQuat Orientation = FQuat(FVector::UpVector, FMath::DegreesToRadians(HeadingValue));
	Orientation = GetReferenceOrientation(WorldUp).Quaternion() * Orientation;
	FVector Location = Orientation * GetEffectiveOffset();
	Location += LastTargetLocation;
	return Location;
}

float UCinemachineOrbitalTransposerComponent::UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis)
{
	return UpdateHeading(DeltaTime, Up, OutAxis, RecenterToTargetHeading, true);
}

float UCinemachineOrbitalTransposerComponent::UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis, FCinemachineAxisStateRecentering& OutRecentering, bool bIsLive)
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();
	if (!IsValid(VCamera))
	{
		OutAxis.Reset();
		OutRecentering.CancelRecentering(this);
		return OutAxis.Value;
	}

	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		OutAxis.MinValue = -180.0F;
		OutAxis.MaxValue = 180.0F;
	}

	if (DeltaTime < 0.0F || !VCamera->GetPreviousStateIsValid() || !bIsLive)
	{
		OutAxis.Reset();
		OutRecentering.CancelRecentering(this);
	}
	else if (OutAxis.Update(this, DeltaTime))
	{
		OutRecentering.CancelRecentering(this);
	}

	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		float FinalHeading = OutAxis.Value;
		OutAxis.Value = 0.0f;
		return FinalHeading;
	}

	float TargetHeading = GetTargetHeading(OutAxis.Value, GetReferenceOrientation(Up));
	OutRecentering.DoRecentering(this, OutAxis, DeltaTime, TargetHeading);
	return OutAxis.Value;
}

void UCinemachineOrbitalTransposerComponent::UpdateInputAxisProvider()
{
	UCinemachineVirtualCameraBaseComponent* VCamera = GetCinemachineVirtualCameraBaseComponent();

	XAxis.SetInputAxisProvider(EAxis::X, nullptr);
	if (!bHeadingIsSlave && IsValid(VCamera))
	{
		if (ICinemachineInputAxisProviderInterface* Provider = VCamera->GetInputAxisProvider())
		{
			XAxis.SetInputAxisProvider(EAxis::X, Provider);
		}
	}
}

float UCinemachineOrbitalTransposerComponent::GetAxisClosestValue(FVector CameraLocation, FVector Up)
{
	FQuat Orientation = GetReferenceOrientation(Up).Quaternion();
	FVector Forward = (Orientation * FVector::ForwardVector).ProjectOnToNormal(Up);
	if (!Forward.IsNearlyZero() && IsValid(GetFollowTarget()))
	{
		float HeadingValue = 0.0F;
		if (BindingMode != ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
		{
			HeadingValue += Heading.Bias;
		}
		Orientation *= FQuat(Up, FMath::DegreesToRadians(HeadingValue));
		FVector TargetLocation = GetFollowTargetLocation();
		FVector Location = TargetLocation + Orientation * GetEffectiveOffset();

		FVector V1 = (Location - TargetLocation).ProjectOnToNormal(Up);
		FVector V2 = (CameraLocation - TargetLocation).ProjectOnToNormal(Up);
		return UVectorExtension::SignedAngle(V1, V2, Up);
	}
	return LastHeading;
}

float UCinemachineOrbitalTransposerComponent::InternalUpdateHeading(UCinemachineOrbitalTransposerComponent* Orbital, float DeltaTime, FVector Up)
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

	return  Orbital->UpdateHeading(DeltaTime, Up, Orbital->XAxis, Orbital->RecenterToTargetHeading, World->GetSubsystem<UCinemachineCoreSubSystem>()->IsLive(Cast<ICinemachineCameraInterface>(Orbital->GetCinemachineVirtualCameraBaseComponent())));
}

float UCinemachineOrbitalTransposerComponent::GetTargetHeading(float CurrentHeading, FRotator TargetOrientation)
{
	if (BindingMode == ECineamchineTransposerBindingMode::SimpleFollowWithWorldUp)
	{
		return 0.0F;
	}

	if (!IsValid(GetFollowTarget()))
	{
		return CurrentHeading;
	}

	ECinemachineOrbitalTransposerHeadingDefinition HeadingDefinition = Heading.Definition;
	if (HeadingDefinition == ECinemachineOrbitalTransposerHeadingDefinition::Velocity && !IsValid(TargetMovementComponent))
	{
		HeadingDefinition = ECinemachineOrbitalTransposerHeadingDefinition::LocationDelta;
	}

	FVector Velocity = FVector::ZeroVector;
	switch (HeadingDefinition)
	{
	case ECinemachineOrbitalTransposerHeadingDefinition::LocationDelta:
		Velocity = GetFollowTargetLocation() - LastTargetLocation;
		break;
	case ECinemachineOrbitalTransposerHeadingDefinition::Velocity:
		Velocity = TargetMovementComponent->Velocity;
		break;
	case ECinemachineOrbitalTransposerHeadingDefinition::TargetForward:
		Velocity = GetFollowTargetRotation().Quaternion() * FVector::ForwardVector;
		break;

	case ECinemachineOrbitalTransposerHeadingDefinition::WorldForward:	// falls through
	default:
		return 0.0F;
	}

	FVector Up = TargetOrientation.Quaternion() * FVector::UpVector;
	Velocity = Velocity.ProjectOnToNormal(Up);
	if (HeadingDefinition != ECinemachineOrbitalTransposerHeadingDefinition::TargetForward)
	{
		int32 FilterSize = Heading.VelocityFilterStrength * 5;
		if (!HeadingTracker.IsValid() || HeadingTracker->FilterSize() != FilterSize)
		{
			HeadingTracker.Reset();
			auto a = FHeadingTracker(Heading.VelocityFilterStrength, FilterSize);
			//HeadingTracker = MakeShared<FHeadingTracker>();
			//HeadingTracker = MakeShareable<FHeadingTracker>(new FHeadingTracker(Heading.VelocityFilterStrength, FilterSize));
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
