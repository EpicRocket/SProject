

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "Component/ActivatableWidgetContainerInterface.h"

#include "CustomActivatableWidget.generated.h"

UCLASS(Abstract, meta = (Category = "Project Only"))
class UCustomActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UCustomActivatableWidget();

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IActivatableWidgetContainerInterface> GetSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const;

	UFUNCTION(BlueprintCallable)
	UCustomActivatableWidget* GetActiveWidget(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const;

	void Close(TFunction<void(bool)> AfterCallback = [](bool) {});

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close"))
	void BP_Close();

	UCustomActivatableWidget* PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<UCustomActivatableWidget> WidgetClass, TFunction<void(UCommonActivatableWidget&)> InstanceInitFunc);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PushWidgetToSubLayer", DeterminesOutputType = "WidgetClass"))
	UCustomActivatableWidget* BP_PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<UCustomActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void RemoveWidgetToSubLayer(UCustomActivatableWidget* RemoveToWidget);

	UFUNCTION(BlueprintCallable)
	void ClearSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer InSubLayerNames);

	UFUNCTION(BlueprintCallable)
	void Clear();

	/* 요청 함수 */
	void RequestPopWidgetToSubLayer(FGameplayTag SubLayerName, TFunction<void(TFunction<void(bool)>)>& OutTask);
	void RequestClearToSubLayer(FGameplayTagContainer InSubLayerNames, TArray<TFunction<void(TFunction<void(bool)>)>>& OutTask);

protected:
	virtual void OnNativeClose(TFunction<void(bool)> AfterCallback);

	void OnCloseInternal();

	UFUNCTION(BlueprintCallable)
	void RegisterSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TScriptInterface<IActivatableWidgetContainerInterface> LayerInterface);

public:
	UPROPERTY(BlueprintReadOnly, meta = (Categories = "UI.Layer"))
	FGameplayTag LayerName;

	// 부모 컨테이너
	UPROPERTY(Transient, BlueprintReadOnly)
	UCustomActivatableWidget* ParentLayerWidget = nullptr;

protected:
	// 자식 컨테이너 맵
	UPROPERTY(Transient)
	TMap<FGameplayTag, TScriptInterface<IActivatableWidgetContainerInterface>> SubLayers;

	TMap<FGameplayTag, TArray<TWeakObjectPtr<UCustomActivatableWidget>>> WidgetPtrMap;
};
