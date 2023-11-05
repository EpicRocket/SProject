
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "SubLayerContainerInterface.generated.h"

class UCommonActivatableWidgetContainerBase;

struct FGameplayTag;

UINTERFACE(BlueprintType)
class USubLayerContainerInterface : public UInterface
{
    GENERATED_BODY()
};

class ISubLayerContainerInterface : public IInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    UCommonActivatableWidgetContainerBase* GetLayerSubContainer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerTag) const;
};
