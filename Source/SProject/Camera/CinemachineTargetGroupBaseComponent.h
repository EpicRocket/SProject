

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineTargetGroupBaseComponent.generated.h"

UCLASS(abstract, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineTargetGroupBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	virtual USceneComponent* GetTarget()
	{
		return this;
	}

	virtual FBox GetBoundingBox()
	{
		return FBox();
	}

	virtual FSphere GetBoundingSphere()
	{
		return FSphere();
	}

	virtual bool IsEmpty()
	{
		return true;
	}

	virtual FBox GetViewSpaceBoundingBox(FMatrix Observer)
	{
		return FBox();
	}

	virtual void GetViewSpaceAngularBounds(FMatrix Observer, FVector2D& MinAngles, FVector2D& MaxAngles, FVector2D ZRange)
	{
		// Empty
	}
};
