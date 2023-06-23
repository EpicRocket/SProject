
#pragma once

#include "CoreMinimal.h"
#include "CinemachineBaseComponent.h"
#include "CinemachineTransposer.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineTransposer : public UCinemachineBaseComponent
{
    GENERATED_BODY()

protected:

    UFUNCTION(BlueprintPure, Category = "Cinemachine")
    FVector GetDamping() const
    {
        return FVector(XDamping, YDamping, ZDamping);
    }
    
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine")
    FVector FollowOffset = FVector::BackwardVector * 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine", meta = (ClampMin = "0.0", ClampMax = "20.0"))
    float XDamping = 1.0F;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine", meta = (ClampMin = "0.0", ClampMax = "20.0"))
    float YDamping = 1.0F;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine", meta = (ClampMin = "0.0", ClampMax = "20.0"))
    float ZDamping = 1.0F;


};
