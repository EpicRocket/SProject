


#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"

#include "ActivatableWidgetContainerInterface.generated.h"

class UCommonActivatableWidget;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UActivatableWidgetContainerInterface : public UInterface
{
    GENERATED_BODY()
};

class IActivatableWidgetContainerInterface : public IInterface
{
    GENERATED_BODY()

public:
    virtual UCommonActivatableWidget* AddWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InstanceInitFunc) = 0;
    virtual void RemoveWidget(UCommonActivatableWidget& WidgetToRemove) = 0;
    virtual void ClearWidgets() = 0;
    virtual int32 GetNumWidgets() const = 0;
    virtual UCommonActivatableWidget* GetActiveWidget() const = 0;
};
