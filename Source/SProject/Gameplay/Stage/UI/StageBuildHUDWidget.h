
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "StageBuildHUDWidget.generated.h"

class AStageBuildZone;

UCLASS()
class MY_API UStageBuildHUDWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void Setup(AStageBuildZone* OwnerBuildZone);

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnSetup();

public:
    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<AStageBuildZone> Owner;
};
