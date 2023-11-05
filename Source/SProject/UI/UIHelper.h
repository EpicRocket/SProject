
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "UIHelper.generated.h"

class APlayerController;
class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

struct FGameplayTag;

UCLASS()
class UUIHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * GameUIPolicy에 선택되어 있는 PrimaryGameLayout으로 부터 선택 된 Layer의 Container Widget를 반환합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidgetContainerBase* GetLayerContainer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

	/**
	 * 선택 된 Layer의 Container에 SubLayer가 존재한다면 해당 Container Widget을 반환합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidgetContainerBase* GetSubLayerContainer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName);

	/**
	 * 선택 된 Layer에서 선택 된 WidgetClass가 존재 한다면 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidget* GetContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	 * 선택 된 SubLayer에서 선택 된 WidgetClass가 존재 한다면 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidget* GetContentFromSubLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	 * 선택 된 Layer에서 활성화 되어 있는 CommonActivatableWidget을 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidget* GetActiveContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

	/**
	 * 선택 된 SubLayer의 활성화 되어 있는 CommonActivatableWidget을 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidget* GetActiveContentFromSubLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName);

	/**
	 * 선택 된 Layer에서 선택 된 WidgetClass가 존재 하는지 확인 합니다.
	 * \return WidgetClass와 동일한 타입의 Widget이 존재한다면 True, 그렇지 않다면 False를 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static bool ExistContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	 * 선택 된 SubLayer에서 선택 된 WidgetClass가 존재 하는지 확인 합니다.
	 * \return WidgetClass와 동일한 타입의 Widget이 존재한다면 True, 그렇지 않다면 False를 반환 합니다.
	 */
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Helper")
	static bool ExistContentFromSubLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	* 선택 된 SubLayer에 Widget을 추가 합니다. 추가 된 위젯은 SubLayer Container에 순차적으로 쌓이게 되며 가장 위에 있는 Widget이 활성화 됩니다.
	* 1. 만약 활성화 된 Widget과 중복 된 WidgetClass라면 새로 생성하지 않고 활성화 되어 있는 Widget을 반환 합니다.
	* 2. 만약 Container에 동일한 WidgetClass가 존재 시 새로 생성하지 않고 해당 Widget을 반환 합니다.
	* \return 활성화 된 Widget
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Helper")
	static UCommonActivatableWidget* PushContentToSubLayer_ForPlayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/**
	 * 선택 된 Layer에서 활성화 된 Widget을 제거 합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Helper")
	static void PopContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

	/**
	 * 선택 된 SubLayer에서 활성화 된 Widget을 제거 합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Helper")
	static void PopContentFromSubLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName);

	/**
	 * 선택 된 Layer에 쌓여 있는 Widget을 모두 제거 합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Helper")
	static void ClearContentFromLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

	/**
	 * 선택 된 SubLayer에 쌓여 있는 Widget을 모두 제거 합니다.
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Helper")
	static void ClearContentFromSubLayer(const APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName);
};
