

#include "BlockingActivatableWidget.h"
// include Plugin
#include "PrimaryGameLayout.h"
// include Project
#include "UI/UIHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlockingActivatableWidget)

bool UBlockingActivatableWidget::IsBlocking() const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UBlockingActivatableWidget, BP_IsBlocking)))
	{
		return BP_IsBlocking();
	}
	return false;
}

void UBlockingActivatableWidget::OnNativeClose(TFunction<void(bool)> AfterCallback)
{
	if (IsBlocking())
	{
		InterrupedTask.Emplace(AfterCallback);
		OnBlocking();
	}
	else
	{
		Super::OnNativeClose(AfterCallback);
	}
}

void UBlockingActivatableWidget::ResumeFlow()
{
	OnCloseInternal();
	if (InterrupedTask.IsSet())
	{
		InterrupedTask.GetValue()(false);
		InterrupedTask.Reset();
	}
}

void UBlockingActivatableWidget::CancelFlow()
{
	if (InterrupedTask.IsSet())
	{
		InterrupedTask.GetValue()(true);
		InterrupedTask.Reset();
	}
}
