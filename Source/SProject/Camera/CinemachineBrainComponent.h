
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CinemachineBlend.h"
#include "CinemachineBrainComponent.generated.h"

UENUM(BlueprintType)
enum class ECinemachineBrainUpdateMethod : uint8
{
    Update,
    SmartUpdate,
    ManualUpdate,
};

class UCinemachineBrainComponent;
class UCinemachineVirtualCameraComponent;
class UCameraComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinemachineBrainEvent, UCinemachineBrainComponent*, Brain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCinemachineVirtualCameraActivatedEvent, UCinemachineVirtualCameraComponent*, VCamera, UCinemachineVirtualCameraComponent*, VCameraFrom);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPROJECT_API UCinemachineBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UCinemachineBrainComponent();

    //~ Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    //~ End UActorComponent Interface

    UFUNCTION(BlueprintPure, Category = CinemachineBrain)
    UCameraComponent* GetOutputCamera();

    UFUNCTION(BlueprintPure, Category = CinemachineBrain)
    FVector DefaultWorldUp() const;

    void ManualUpdate(float DeltaTime);

	bool IsLiveBlend(UObject* ICamera);

	bool IsLive(UObject* ICamera, bool DominantChildOnly = false);



public:
	UPROPERTY(BlueprintAssignable, Category = CinemachineBrain)
	FCinemachineBrainEvent CameraCutEvent;

	UPROPERTY(BlueprintAssignable, Category = CinemachineBrain)
	FCinemachineVirtualCameraActivatedEvent CameraActivatedEvent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineBrain)
    ECinemachineBrainUpdateMethod UpdateMethod;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineBrain)
    FCinemachineBlendDefinition DefaultBlend;

private:
    UPROPERTY(VisibleAnywhere, Category = CinemachineBrain, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* OutputCamera;

    UPROPERTY(VisibleAnywhere, Category = CinemachineBrain, meta = (AllowPrivateAccess = "true"))
    AActor* TargetOverride;
};
