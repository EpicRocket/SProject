

#include "CinemachineTargetGroupComponent.h"
#include "Shared/VectorExtension.h"

static FBox CreateBox(FVector Min, FVector Max)
{
	FVector Extents = (Max - Min) * 0.5F;
	return FBox::BuildAABB(Min + Extents, Extents);
}

static void EncapsulateBox(FBox& Source, FVector Point)
{
	Source = CreateBox(FVector::Min(Source.Min, Point), FVector::Max(Source.Max, Point));
};

static void EncapsulateBox(FBox& Source, const FBox& Other)
{
	FVector Center = Other.GetCenter();
	FVector Extent = Other.GetExtent();
	EncapsulateBox(Source, Center - Extent);
	EncapsulateBox(Source, Center + Extent);
}

UCinemachineTargetGroupComponent::UCinemachineTargetGroupComponent()
	: LocationMode(ECinemachineTargetGroupLocationMode::GroupCenter)
	, RotationMode(ECinemachineTargetGroupRotationMode::Manual)
	, MaxWeight(0.0F)
	, AverageLocation(FVector::ZeroVector)
	, BoundingBox(ForceInitToZero)
	, BoundingSphere(ForceInitToZero)
	, LastUpdateFrame(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCinemachineTargetGroupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TickType == ELevelTick::LEVELTICK_All)
	{
		DoUpdate();
	}
}

FBox UCinemachineTargetGroupComponent::GetBoundingBox()
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}
	return BoundingBox;
}

FSphere UCinemachineTargetGroupComponent::GetBoundingSphere()
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}
	return BoundingSphere;
}

bool UCinemachineTargetGroupComponent::IsEmpty()
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}
	return ValidMembers.IsEmpty();
}

FBox UCinemachineTargetGroupComponent::GetViewSpaceBoundingBox(FMatrix Observer)
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}

	FMatrix InversView = Observer;
	if (!(Observer.GetScaledAxis(EAxis::X).IsNearlyZero(SMALL_NUMBER) || Observer.GetScaledAxis(EAxis::Y).IsNearlyZero(SMALL_NUMBER) || Observer.GetScaledAxis(EAxis::Z).IsNearlyZero(SMALL_NUMBER)))
	{
		InversView = Observer.InverseFast();
	}

	FBox Result = FBox::BuildAABB(InversView.TransformPosition(AverageLocation), FVector::ZeroVector);
	if (CachedCountIsValid())
	{
		bool GotOne = false;
		FVector Unit = FVector::OneVector * 2.0F;
		for (int32 Index : ValidMembers)
		{
			FSphere Sphere = WeightedMemberBoundsForValidMember(Members[Index], AverageLocation, MaxWeight);
			Sphere.Center = InversView.TransformPosition(Sphere.Center);
			if (GotOne)
			{
				EncapsulateBox(Result, FBox::BuildAABB(Sphere.Center, Sphere.W * Unit));
			}
			else
			{
				Result = FBox::BuildAABB(Sphere.Center, Sphere.W * Unit);
			}
			GotOne = true;
		}
	}
	return Result;
}

void UCinemachineTargetGroupComponent::GetViewSpaceAngularBounds(FMatrix Observer, FVector2D& MinAngles, FVector2D& MaxAngles, FVector2D ZRange)
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}

	FMatrix WorldToLocal = Observer;
	if (!(Observer.GetScaledAxis(EAxis::X).IsNearlyZero(SMALL_NUMBER) || Observer.GetScaledAxis(EAxis::Y).IsNearlyZero(SMALL_NUMBER) || Observer.GetScaledAxis(EAxis::Z).IsNearlyZero(SMALL_NUMBER)))
	{
		WorldToLocal = Observer.InverseFast();
	}

	ZRange = FVector2D::ZeroVector;
	FBox Box = FBox();
	if (CachedCountIsValid())
	{
		bool HaveOne = false;
		int32 Count = ValidMembers.Num();
		for (int32 i = 0; i < Count; ++i)
		{
			FSphere Sphere = WeightedMemberBoundsForValidMember(Members[ValidMembers[i]], AverageLocation, MaxWeight);
			FVector Point = WorldToLocal.TransformPosition(Sphere.Center);
			if (Point.X < UE_KINDA_SMALL_NUMBER)
			{
				continue;
			}

			float N = Sphere.W / Point.X;
			FVector N2 = FVector(N, N, 0.0F);
			FVector PointN = Point / Point.X;
			if (!HaveOne)
			{
				Box = FBox::BuildAABB(PointN, N2);
				ZRange = FVector2D(Point.X, Point.X);
				HaveOne = true;
			}
			else
			{
				EncapsulateBox(Box, PointN + N2);
				EncapsulateBox(Box, PointN - N2);
				ZRange.X = FMath::Min(ZRange.X, Point.X);
				ZRange.Y = FMath::Max(ZRange.Y, Point.X);
			}
		}
	}
	MinAngles = FVector2D(UVectorExtension::SignedAngle(FVector::ForwardVector, FVector(0.0F, Box.Min.Y, 1.0F), FVector::LeftVector), UVectorExtension::SignedAngle(FVector::ForwardVector, FVector(Box.Min.X, 0.0F, 1.0F), FVector::UpVector));
	MaxAngles = FVector2D(UVectorExtension::SignedAngle(FVector::ForwardVector, FVector(0.0F, Box.Max.Y, 1.0F), FVector::LeftVector), UVectorExtension::SignedAngle(FVector::ForwardVector, FVector(Box.Max.X, 0.0F, 1.0F), FVector::UpVector));
}

void UCinemachineTargetGroupComponent::DoUpdate()
{
	LastUpdateFrame = GFrameCounter;

	UpdateMemberValidity();
	AverageLocation = CalculateAverageLocation(MaxWeight);
	BoundingBox = CalculateBoundingBox();
	BoundingSphere = CalculateBoundingSphere(MaxWeight);

	switch (LocationMode)
	{
	case ECinemachineTargetGroupLocationMode::GroupCenter:
		SetWorldLocation(Bounds.Origin);
		break;
	case ECinemachineTargetGroupLocationMode::GroupAverage:
		SetWorldLocation(AverageLocation);
		break;
	}

	switch (RotationMode)
	{
	case ECinemachineTargetGroupRotationMode::Manual:
		break;
	case ECinemachineTargetGroupRotationMode::GroupAverage:
		SetWorldRotation(CalculateAverageOrientation());
		break;
	}
}

void UCinemachineTargetGroupComponent::AddMember(USceneComponent* Target, float Weight, float Radius)
{
	FCinemachineTargetGroupMember Member;
	Member.Target = Target;
	Member.Weight = Weight;
	Member.Radius = Radius;
	Members.Emplace(Member);
}

void UCinemachineTargetGroupComponent::RemoveMember(USceneComponent* Target)
{
	Members.RemoveAll([Target](const FCinemachineTargetGroupMember& Member)
					  { return Member.Target == Target; });
}

FSphere UCinemachineTargetGroupComponent::GetWeightedBoundsForMember(int32 MemberIndex)
{
	if (LastUpdateFrame != GFrameCounter)
	{
		DoUpdate();
	}
	if (!IndexIsValid(MemberIndex) || !MemberValidity[MemberIndex])
	{
		return BoundingSphere;
	}
	return WeightedMemberBoundsForValidMember(Members[MemberIndex], AverageLocation, MaxWeight);
}

FSphere UCinemachineTargetGroupComponent::WeightedMemberBoundsForValidMember(FCinemachineTargetGroupMember& OutMember, FVector TargetLocation, float Max)
{
	FVector Location = OutMember.Target->GetComponentLocation();
	float Weight = FMath::Max(0.0F, OutMember.Weight);
	if (Max > UE_KINDA_SMALL_NUMBER && Weight < Max)
	{
		Weight /= Max;
	}
	else
	{
		Weight = 1.0F;
	}
	return FSphere(FMath::Lerp(TargetLocation, Location, Weight), OutMember.Radius * Weight);
}

void UCinemachineTargetGroupComponent::UpdateMemberValidity()
{
	ValidMembers.Empty();
	MemberValidity.Empty();
	for (int32 i = 0; i < Members.Num(); ++i)
	{
		bool bValue = IsValid(Members[i].Target) && Members[i].Weight > UE_KINDA_SMALL_NUMBER;
		MemberValidity.Emplace(bValue);
		if (bValue)
		{
			ValidMembers.Emplace(i);
		}
	}
}

FVector UCinemachineTargetGroupComponent::CalculateAverageLocation(float& OutMax)
{
	FVector Location = FVector::ZeroVector;
	float WeightSum = 0.0F;
	OutMax = 0.0F;
	int32 Count = ValidMembers.Num();
	for (int32 i = 0; i < Count; ++i)
	{
		int32 Index = ValidMembers[i];
		FCinemachineTargetGroupMember& Member = Members[Index];
		float Weight = Member.Weight;
		WeightSum += Weight;
		Location += Member.Target->GetComponentLocation() * Weight;
		OutMax = FMath::Max(OutMax, Weight);
	}
	if (WeightSum > UE_KINDA_SMALL_NUMBER)
	{
		Location /= WeightSum;
	}
	else
	{
		Location = GetComponentLocation();
	}
	return Location;
}

FBox UCinemachineTargetGroupComponent::CalculateBoundingBox()
{
	FBox Result = FBox::BuildAABB(AverageLocation, FVector::ZeroVector);
	if (MaxWeight > UE_KINDA_SMALL_NUMBER)
	{
		FVector Unit = 2.0F * FVector::OneVector;
		for (int32 Index : ValidMembers)
		{
			FSphere Sphere = WeightedMemberBoundsForValidMember(Members[Index], AverageLocation, MaxWeight);
			EncapsulateBox(Result, FBox::BuildAABB(Sphere.Center, Sphere.W * Unit));
		}
	}
	return Result;
}

FSphere UCinemachineTargetGroupComponent::CalculateBoundingSphere(float Max)
{
	FSphere Result = FSphere(AverageLocation, 0.0F);
	bool GotOne = false;

	for (int32 Index : ValidMembers)
	{
		FSphere Sphere = WeightedMemberBoundsForValidMember(Members[Index], AverageLocation, Max);
		if (!GotOne)
		{
			GotOne = true;
			Result = Sphere;
			continue;
		}
		float Distance = (Sphere.Center - Result.Center).SizeSquared() + Sphere.W;
		if (Distance > Result.W)
		{
			Result.W = (Distance + Result.W) * 0.5F;
			Result.Center = (Result.W * Result.Center + (Distance - Result.W) * Sphere.Center) / Distance;
		}
	}
	return Result;
}

FRotator UCinemachineTargetGroupComponent::CalculateAverageOrientation()
{
	if (MaxWeight <= UE_KINDA_SMALL_NUMBER)
	{
		return GetComponentRotation();
	}

	float WeightAverage = 0.0F;
	FQuat Rotation = FQuat::Identity;
	for (int32 Index : ValidMembers)
	{
		FCinemachineTargetGroupMember& Member = Members[Index];
		float ScaledWeight = Member.Weight / MaxWeight;
		FRotator MemberRotation = Member.Target->GetComponentRotation();
		Rotation *= FQuat::Slerp(FQuat::Identity, MemberRotation.Quaternion(), ScaledWeight);
		WeightAverage += ScaledWeight;
	}
	return FQuat::Slerp(FQuat::Identity, Rotation, 1.0F / WeightAverage).Rotator();
}

bool UCinemachineTargetGroupComponent::CachedCountIsValid() const
{
	return MemberValidity.Num() == Members.Num();
}

bool UCinemachineTargetGroupComponent::IndexIsValid(int32 Index) const
{
	return Index >= 0 && Index < Members.Num() && CachedCountIsValid();
}
