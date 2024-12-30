
#pragma once

#include "CoreMinimal.h"
#include "Error/GErrorTypes.h"

#include "GMessage.generated.h"

USTRUCT(BlueprintType)
struct FGMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObject> Instigator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGErrorInfo ErrorInfo;
};
