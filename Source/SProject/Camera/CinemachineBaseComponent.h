
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CinemachineBaseComponent.generated.h"

class AActor;
class UCinemachineVirtualCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineBaseComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    virtual float GetMaxDampTime()
    {
        return 0.0F;
    }

    AActor* GetFollowTarget();

    AActor* GetLookAtTarget();
    

protected:

    UCinemachineVirtualCameraComponent* CameraComponent;
};
