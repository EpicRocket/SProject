
#include "GameplayPathActor.h"
// include Engine
#include "Components/SplineComponent.h"


const FName AGameplayPathActor::PathTagName = TEXT("Path");

FVector AGameplayPathActor::GetClosestPoint(const FVector& SourceLocation) const
{
	auto Spline = GetSpline();
	if (Spline == nullptr)
	{
		return GetActorLocation();
	}
	float InputKey = Spline->FindInputKeyClosestToWorldLocation(SourceLocation);
	return Spline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
}

bool AGameplayPathActor::IsAtEnd(const FVector& SourceLocation) const
{
	auto Spline = GetSpline();
	if (Spline == nullptr)
	{
		return false;
	}

	// TODO: 구현 필요

	return false;
}
