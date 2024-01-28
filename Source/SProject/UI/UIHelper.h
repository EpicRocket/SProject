

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "UIHelper.generated.h"

class APlayerController;
class UCommonActivatableWidget;
class UCustomActivatableWidget;
class UGameLayout;
class UUserWidget;

struct FGameplayTag;
struct FGameplayTagContainer;

UCLASS()
class UUIHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "WidgetClass"))
	static UCustomActivatableWidget* PushContentToLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCustomActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "WidgetClass"))
	static UCustomActivatableWidget* PushContentToSubLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UCustomActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	static void RemoveContentToLayer_ForPlayer(APlayerController* PlayerController, UCustomActivatableWidget* RemoveToWidget);

	UFUNCTION(BlueprintCallable)
	static void PopContentToLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName);

	UFUNCTION(BlueprintCallable)
	static void PopContentToTopSubLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag SubLayerName);

	UFUNCTION(BlueprintCallable)
	static void ClearLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTagContainer LayerNames);

	UFUNCTION(BlueprintCallable)
	static void ClearTopSubLayer_ForPlayer(APlayerController* PlayerController, UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTagContainer SubLayerNames);

	/**
	 * 값을 축소 단위로 치환 합니다.
	 * 
	 * \param Value 표시할 값
	 * \param Digits 표시 할 최대 소수점 자릿수
	 * \return 축소 단위로 치환 된 값
	 */
	UFUNCTION(BlueprintPure)
	static FText FormatWithSuffix(int64 Value, int32 Digits = 2);
};
