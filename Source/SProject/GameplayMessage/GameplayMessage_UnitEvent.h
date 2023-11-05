
#pragma once

#include "CoreMinimal.h"

#include "GameplayMessage_UnitEvent.generated.h"

class AUnit;

USTRUCT(BlueprintType)
struct FGameplayMessage_UnitEvent
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AUnit* Source = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AUnit* Destination = nullptr;
};