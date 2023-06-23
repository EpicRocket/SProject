
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineTargetGroupBaseComponent.generated.h"

UCLASS(abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineTargetGroupBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	virtual AActor* GetTarget()
	{
		return nullptr;
	}

	virtual FBoxSphereBounds GetBounds()
	{
		return FBoxSphereBounds();
	}

	virtual bool IsEmpty()
	{
		return true;
	}

	virtual FBox GetViewSpaceBoundingBox(FMatrix Observer)
	{
		return FBox();
	}

	virtual void GetViewSpaceAngularBounds(FMatrix Observer, FVector& MaxAngles, FVector ZRange)
	{
		// Empty
	}
};
