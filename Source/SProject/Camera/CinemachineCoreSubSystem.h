
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinemachineBlend.h"
#include "CinemachineCoreSubSystem.generated.h"

UENUM(BlueprintType)
enum class ECinemachineStage : uint8
{
    Body,
    Aim,
    Noise,
    Finalize,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCinemachineBlendDelegate, UObject*, FromVCam, UObject*, ToVCam, FCinemachineBlendDefinition, DefaultBlend, UObject*, Owner);

UCLASS()
class SPROJECT_API UCinemachineCoreSubSystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UCinemachineCoreSubSystem();

    UPROPERTY(BlueprintAssignable, Category = "Cinemachine")
    FCinemachineBlendDelegate OnBlend;

};
