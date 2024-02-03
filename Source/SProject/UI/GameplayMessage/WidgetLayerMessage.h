
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "WidgetLayerMessage.generated.h"

class USubActivatableWidget;

USTRUCT(BlueprintType)
struct FWidgetLayerMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag GameplayTag;

    UPROPERTY(BlueprintReadOnly)
    USubActivatableWidget* ParentWidget;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentStack;

    UPROPERTY(BlueprintReadOnly)
    class USubActivatableWidget* CurrentWidget;
};
