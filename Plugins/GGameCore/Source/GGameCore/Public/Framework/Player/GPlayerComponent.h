
#pragma once

#include "Components/GameFrameworkComponent.h"

#include "GPlayerComponent.generated.h"

class APlayerController;
class ULocalPlayer;

UCLASS()
class GGAMECORE_API UGPlayerComponent : public UGameFrameworkComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    virtual APlayerController* GetOwningPlayer() const;

    template <class T>
    T* GetOwningPlayer() const
    {
        return Cast<T>(GetOwningPlayer());
    }

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    virtual ULocalPlayer* GetOwningLocalPlayer() const;

    template <class T>
    T* GetOwningLocalPlayer() const
    {
        return Cast<T>(GetOwningLocalPlayer());
    }
};
