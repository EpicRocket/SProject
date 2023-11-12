
#include "WaypointPather.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WaypointPather)

AWaypointPather::AWaypointPather()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(SceneComponent);
}

FVector AWaypointPather::GetPathLocation(float Alpha) const
{
	if (!IsValid(SplineComponent))
	{
		return GetActorLocation();
	}

	float SplineLength = SplineComponent->GetSplineLength();
	float Distance = FMath::Clamp(Alpha, 0.0f, 1.0f) * SplineLength;
	return SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
}
