// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Loading/UI/GLoadingWidget.h"
// include Engine
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/WidgetAnimationEvents.h"
#include "Animation/UMGSequencePlayer.h"
// C:\UnrealEngine\Engine\Source\Runtime\UMG\Public\Animation\UMGSequencePlayer.h

#include UE_INLINE_GENERATED_CPP_BY_NAME(GLoadingWidget)

#if WITH_EDITOR
const FText UGLoadingWidget::GetPaletteCategory()
{
	return FText::FromString(TEXT("Loading"));
}
#endif

void UGLoadingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(BeginAnimation))
	{
		{
			FWidgetAnimationDynamicEvent NewEvent;
			NewEvent.BindDynamic(this, &UGLoadingWidget::OnBeginAnimationStarted);
			BindToAnimationStarted(
				BeginAnimation,
				NewEvent
			);
		}

		{
			FWidgetAnimationDynamicEvent NewEvent;
			NewEvent.BindDynamic(this, &UGLoadingWidget::OnBeginAnimationFinished);
			BindToAnimationFinished(
				BeginAnimation,
				NewEvent
			);
		}
	}

	if (IsValid(EndAnimation))
	{
		{
			FWidgetAnimationDynamicEvent NewEvent;
			NewEvent.BindDynamic(this, &UGLoadingWidget::OnEndAnimationStarted);
			BindToAnimationStarted(
				EndAnimation,
				NewEvent
			);
		}

		{
			FWidgetAnimationDynamicEvent NewEvent;
			NewEvent.BindDynamic(this, &UGLoadingWidget::OnEndAnimationFinished);
			BindToAnimationFinished(
				EndAnimation,
				NewEvent
			);
		}
	}
}

void UGLoadingWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (IsValid(BeginAnimation))
	{
		PlayAnimation(BeginAnimation);
	}
}

void UGLoadingWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	// NOTE. 시작 애니메이션이 아직 재생중이라면 종료 애니메이션을 재생하지 않게 합니다.
	if (UUMGSequencePlayer* FoundPlayer = GetSequencePlayer(BeginAnimation))
	{
		if (FoundPlayer->IsPlayingForward())
		{
			return;
		}
	}

	if (IsValid(EndAnimation))
	{
		PlayAnimation(EndAnimation);
	}
}

bool UGLoadingWidget::IsExistBeginAnimation() const
{
	return IsValid(BeginAnimation);
}

void UGLoadingWidget::BindToBeginAnimationStarted(const FOnLoadingWidgetAnimationDelegate& Task)
{
	if (Task.IsBound())
	{
		BeginAnimationFinishedTasks.Add(Task);
	}
}

void UGLoadingWidget::BindToBeginAnimationFinished(const FOnLoadingWidgetAnimationDelegate& Task)
{
	if (Task.IsBound())
	{
		BeginAnimationFinishedTasks.Add(Task);
	}
}

bool UGLoadingWidget::IsExistEndAnimation() const
{
	return IsValid(EndAnimation);
}

void UGLoadingWidget::BindToEndAnimationStarted(const FOnLoadingWidgetAnimationDelegate& Task)
{
	if (Task.IsBound())
	{
		EndAnimationFinishedTasks.Add(Task);
	}
}

void UGLoadingWidget::BindToEndAnimationFinished(const FOnLoadingWidgetAnimationDelegate& Task)
{
	if (Task.IsBound())
	{
		EndAnimationFinishedTasks.Add(Task);
	}
}

void UGLoadingWidget::OnBeginAnimationStarted()
{
	for (auto& Task : BeginAnimationStartedTasks)
	{
		if (Task.IsBound())
		{
			Task.ExecuteIfBound();
		}
	}
}

void UGLoadingWidget::OnBeginAnimationFinished()
{
	for (auto& Task : BeginAnimationFinishedTasks)
	{
		if (Task.IsBound())
		{
			Task.ExecuteIfBound();
		}
	}

	if (!IsActivated())
	{
		PlayAnimation(EndAnimation);
	}
}

void UGLoadingWidget::OnEndAnimationStarted()
{
	for (auto& Task : EndAnimationStartedTasks)
	{
		if (Task.IsBound())
		{
			Task.ExecuteIfBound();
		}
	}
}

void UGLoadingWidget::OnEndAnimationFinished()
{
	for (auto& Task : EndAnimationFinishedTasks)
	{
		if (Task.IsBound())
		{
			Task.ExecuteIfBound();
		}
	}
}
