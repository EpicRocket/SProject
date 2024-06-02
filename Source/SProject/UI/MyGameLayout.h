
#pragma once

#include "UI/PrimaryGameLayout.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "MyGameLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

UCLASS(Abstract, meta = (DisableNativeTick))
class SPROJECT_API UMyGameLayout : public UPrimaryGameLayout
{
    GENERATED_BODY()

public:
    static UMyGameLayout* Get(const ULocalPlayer* LocalPlayer);

	virtual void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning) override;
};

UCLASS()
class SPROJECT_API UMyGameLayoutHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FName SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason);

    static void ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken);

    UFUNCTION(BlueprintCallable, Category = "UI", meta = (DisplayName = "Push Content To Layer", DeterminesOutputType = WidgetClass))
    static UCommonActivatableWidget* BP_PushContentToLayer(const ULocalPlayer* LocalPlayer, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);
};
