
#pragma once

#include "CoreMinimal.h"
#include "Error/GErrorTypes.h"

#include "GGameplayMessage.generated.h"

USTRUCT(BlueprintType)
struct FGGameplayMessage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObject> Instigator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGErrorInfo ErrorInfo;
};
