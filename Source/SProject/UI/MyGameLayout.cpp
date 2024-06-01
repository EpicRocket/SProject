
#include "MyGameLayout.h"

#include "MyUIHelper.h"
#include "MyUIPolicy.h"
#include "GameFramework/MyLocalPlayer.h"

void UMyGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		const FName SuspendToken = UMyUIHelper::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion"));
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const FName SuspendToken = SuspendInputTokens.Pop();
			UMyUIHelper::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}
