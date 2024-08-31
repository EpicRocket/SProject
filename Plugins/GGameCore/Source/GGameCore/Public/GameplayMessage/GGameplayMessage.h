
#pragma once

#include "CoreMinimal.h"
#include "System/GErrorTypes.h"

#include "GGameplayMessage.generated.h"

USTRUCT(BlueprintType)
struct FGGameplayMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGErrorInfo ErrorInfo;
};
