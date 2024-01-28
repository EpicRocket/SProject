
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "WidgetLayerMessage.generated.h"

USTRUCT(BlueprintType)
struct FWidgetLayerMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag GameplayTag;

    UPROPERTY(BlueprintReadOnly)
    TScriptInterface<class IActivatableWidgetContainerInterface> Layer;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentStack;

    UPROPERTY(BlueprintReadOnly)
    class UCustomActivatableWidget* CurrentWidget;
};
