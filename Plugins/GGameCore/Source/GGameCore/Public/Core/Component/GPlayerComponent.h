
#pragma once

#include "Core/Component/GGameCoreComponent.h"

#include "GPlayerComponent.generated.h"

class APlayerController;
class ULocalPlayer;
struct FLatentActionInfo;
struct FGErrorInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class GGAMECORE_API UGPlayerComponent : public UGGameCoreComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    virtual APlayerController* GetOwningPlayer() const;

    template <class T>
    T* GetOwningPlayer() const
    {
        static_assert(TPointerIsConvertibleFromTo<T, APlayerController>::Value, "GetOwningPlayer의 'T' 템플릿 파라미터는 APlayerController에서 파생되어야 합니다.");
        return Cast<T>(GetOwningPlayer());
    }

    UFUNCTION(BlueprintCosmetic)
    virtual ULocalPlayer* GetOwningLocalPlayer() const;

    template <class T>
    T* GetOwningLocalPlayer() const
    {
        static_assert(TPointerIsConvertibleFromTo<T, ULocalPlayer>::Value, "GetOwningLocalPlayer의 'T' 템플릿 파라미터는 ULocalPlayer에서 파생되어야 합니다.");
        return Cast<T>(GetOwningLocalPlayer());
    }

};
