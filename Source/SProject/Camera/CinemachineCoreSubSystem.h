
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CinemachineCoreSubSystem.generated.h"

UENUM(BlueprintType)
enum class ECinemachineStage : uint8
{
    Body,
    Aim,
    Noise,
    Finalize,
};


UCLASS()
class SPROJECT_API UCinemachineCoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UCinemachineCoreSubSystem();
    
};
