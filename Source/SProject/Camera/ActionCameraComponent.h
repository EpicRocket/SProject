
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "ActionCameraComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPROJECT_API UActionCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UActionCameraComponent();

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface
    
    // function to return to the default state
    void Recall();

    // function to set the target
    void SetTarget(AActor* InTarget);

    
protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "카메라를 소유한 액터"))
    AActor* Owner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "바라볼 대상"))
    AActor* LookAtTarget;
    
};

