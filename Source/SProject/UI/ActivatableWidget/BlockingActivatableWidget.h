

#pragma once

#include "UI/SubActivatableWidget.h"

#include "BlockingActivatableWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FBlockingActivatableWidgetDelegate, bool, bCancel);

/**
 * @brief 이 위젯이 GameLayout에 의하여 제거 될 시, 블로킹이 가능한 위젯 인지 확인 합니다.
 *          Async Action에 의해 정상 작동 됩니다.
 */
UCLASS(Abstract, meta = (Category = "Project Only"))
class UBlockingActivatableWidget : public USubActivatableWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    bool IsBlocking() const;

protected:
    virtual void OnNativeClose(TFunction<void(bool)> AfterCallback);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    bool BP_IsBlocking() const;

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnBlocking();

    UFUNCTION(BlueprintCallable)
    void ResumeFlow();

    UFUNCTION(BlueprintCallable)
    void CancelFlow();

protected:
    TOptional<TFunction<void(bool)>> InterrupedTask;
};
