
#include "GameplayPathActor.h"
// include Engine
#include "Components/SplineComponent.h"


const FName AGameplayPathActor::PathTagName = TEXT("Path");

FVector AGameplayPathActor::GetLocationAtInputKey(float InputKey) const
{
	auto Spline = GetSpline();
	if (Spline == nullptr)
	{
		return GetActorLocation();
	}
	return Spline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
}

float AGameplayPathActor::GetClosestInputKey(const FVector& Location) const
{
	auto Spline = GetSpline();
	if (Spline == nullptr)
	{
		return 0.0F;
	}
	return Spline->FindInputKeyClosestToWorldLocation(Location);
}

bool AGameplayPathActor::IsAtEnd(const FVector& Location, float Radius)
{
	auto Spline = GetSpline();
	if (Spline == nullptr)
	{
		return 0.0F;
	}
	auto LastPoint = Spline->GetLocationAtTime(Spline->Duration, ESplineCoordinateSpace::World);
	return FVector::Dist(Location, LastPoint) < Radius;
}
