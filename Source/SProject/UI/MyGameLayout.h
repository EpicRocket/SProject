
#pragma once

#include "UI/PrimaryGameLayout.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "MyGameLayout.generated.h"

class UCommonActivatableWidgetContainerBase;
class APlayerController;

UCLASS(Abstract, meta = (DisableNativeTick))
class MY_API UMyGameLayout : public UPrimaryGameLayout
{
    GENERATED_BODY()

public:
    static UMyGameLayout* Get(const ULocalPlayer* LocalPlayer);

	virtual void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning) override;
};

UCLASS()
class MY_API UMyGameLayoutHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FName SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason);

    static void ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken);

    UFUNCTION(BlueprintCallable, Category = "UI", meta = (DeterminesOutputType = WidgetClass))
    static UCommonActivatableWidget* PushContentToLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

    UFUNCTION(BlueprintCallable, Category = "UI")
    static void RemoveContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UCommonActivatableWidget* WidgetToRemove);

    UFUNCTION(BlueprintCallable, Category = "UI")
    static void RemoveContent(const APlayerController* PlayerController, UCommonActivatableWidget* WidgetToRemove);

    UFUNCTION(BlueprintCallable, Category = "UI")
    static void ClearContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

    UFUNCTION(BlueprintCallable, Category = "UI")
    static void ClearContent(const APlayerController* PlayerController);
};
