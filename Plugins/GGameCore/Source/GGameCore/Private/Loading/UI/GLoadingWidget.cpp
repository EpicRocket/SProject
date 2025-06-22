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
		FWidgetAnimationDynamicEvent NewEvent;
		NewEvent.BindDynamic(this, &UGLoadingWidget::OnBeginAnimationFinished);
		BindToAnimationFinished(
			BeginAnimation,
			NewEvent
		);
	}

	if (IsValid(EndAnimation))
	{
		FWidgetAnimationDynamicEvent NewEvent;
		NewEvent.BindDynamic(this, &UGLoadingWidget::OnEndAnimationFinished);
		BindToAnimationFinished(
			EndAnimation,
			NewEvent
		);
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

bool UGLoadingWidget::IsExistBeginAnimationTask() const
{
	return BeginAnimationFinishedTasks.Num() > 0;
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

bool UGLoadingWidget::IsExistEndAnimationTask() const
{
	return EndAnimationFinishedTasks.Num() > 0;
}

void UGLoadingWidget::BindToEndAnimationFinished(const FOnLoadingWidgetAnimationDelegate& Task)
{
	if (Task.IsBound())
	{
		EndAnimationFinishedTasks.Add(Task);
	}
}

void UGLoadingWidget::OnBeginAnimationFinished()
{
	if (IsActivated())
	{
		for (auto& Task : BeginAnimationFinishedTasks)
		{
			if (Task.IsBound())
			{
				Task.ExecuteIfBound();
			}
		}
	}
	else if (IsValid(EndAnimation))
	{
		PlayAnimation(EndAnimation);
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
