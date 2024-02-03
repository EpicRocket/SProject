

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "Component/ActivatableWidgetContainerInterface.h"

#include "SubActivatableWidget.generated.h"

UCLASS(Abstract, meta = (Category = "Project Only"))
class USubActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USubActivatableWidget();

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IActivatableWidgetContainerInterface> GetSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const;

	UFUNCTION(BlueprintCallable)
	USubActivatableWidget* GetActiveWidget(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName) const;

	void Close(TFunction<void(bool)> AfterCallback = [](bool) {});

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close"))
	void BP_Close();

	USubActivatableWidget* PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<USubActivatableWidget> WidgetClass, TFunction<void(UCommonActivatableWidget&)> InstanceInitFunc);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PushWidgetToSubLayer", DeterminesOutputType = "WidgetClass"))
	USubActivatableWidget* BP_PushWidgetToSubLayer(UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName, TSubclassOf<USubActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void RemoveWidgetToSubLayer(USubActivatableWidget* RemoveToWidget);

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
	USubActivatableWidget* ParentLayerWidget = nullptr;

protected:
	// 자식 컨테이너 맵
	UPROPERTY(Transient)
	TMap<FGameplayTag, TScriptInterface<IActivatableWidgetContainerInterface>> SubLayers;

	TMap<FGameplayTag, TArray<TWeakObjectPtr<USubActivatableWidget>>> WidgetPtrMap;
};
