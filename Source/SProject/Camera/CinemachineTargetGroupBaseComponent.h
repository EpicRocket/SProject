

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineTargetGroupBaseComponent.generated.h"

UCLASS(abstract, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineTargetGroupBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FBox GetBoundingBox()
	{
		return FBox();
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FSphere GetBoundingSphere()
	{
		return FSphere();
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual bool IsEmpty()
	{
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FBox GetViewSpaceBoundingBox(FMatrix Observer)
	{
		return FBox();
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual void GetViewSpaceAngularBounds(FMatrix Observer, FVector2D& MinAngles, FVector2D& MaxAngles, FVector2D ZRange)
	{
		// Empty
	}
};
