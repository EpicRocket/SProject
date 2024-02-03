

#include "AsyncAction_PopContentToSubLayer.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
// include Plugin
#include "PrimaryGameLayout.h"
#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"
// include Project
#include "UI/SubActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PopContentToSubLayer)

UAsyncAction_PopContentToSubLayer *UAsyncAction_PopContentToSubLayer::PopContentToSubLayer(USubActivatableWidget* ParentWidget, UPARAM(meta = (Categories = "UI.SubLayer")) FGameplayTag InSubLayerName)
{
    if (UWorld *World = GEngine->GetWorldFromContextObject(ParentWidget, EGetWorldErrorMode::LogAndReturnNull))
    {
        UAsyncAction_PopContentToSubLayer *Action = NewObject<UAsyncAction_PopContentToSubLayer>();
        Action->LayerName = InSubLayerName;
        Action->ParentWidgetPtr = ParentWidget;
        return Action;
    }
    return nullptr;
}

void UAsyncAction_PopContentToSubLayer::Activate()
{
    if (ParentWidgetPtr.IsValid())
    {
        ParentWidgetPtr->RequestPopWidgetToSubLayer(LayerName, Task);
        if (Task)
        {
            Task([this, ThisPtr = TWeakObjectPtr<UAsyncAction_PopContentToSubLayer>(this)](bool bCancel)
                {
                    if (ThisPtr.IsValid())
                    {
                        if (bCancel)
                        {
                            OnCancel.Broadcast();
                        }
                        else
                        {
                            OnComplete.Broadcast();
                        }
                    }
                    SetReadyToDestroy();
                });
        }
        else
        {
            OnComplete.Broadcast();
            SetReadyToDestroy();
        }
    }
    else
    {
        SetReadyToDestroy();
    }
}
