
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IUpdatableWidget.generated.h"

UINTERFACE(BlueprintType)
class GGAMECORE_API UUpdatableWidget : public UInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IUpdatableWidget : public IInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void UpdateUI();
    virtual void UpdateUI_Implementation() {}

};

