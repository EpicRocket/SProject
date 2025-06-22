// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "GLoadingWidget.generated.h"

class UWidgetAnimation;

DECLARE_DELEGATE(FOnLoadingWidgetAnimationDelegate);

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, Category = "Loading", ClassGroup = "Loading", meta = (DisableNativeTick, DontUseGenericSpawnObject = "true"))
class GGAMECORE_API UGLoadingWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// UWidget
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	// ~UWidget
	
protected:
	// UUserWidget
	virtual void NativeOnInitialized();
	// ~UUserWidget

	// UCommonActivatableWidget
	virtual void NativeOnActivated();
	virtual void NativeOnDeactivated();
	// ~UCommonActivatableWidget

public:
	bool IsExistBeginAnimation() const;
	bool IsExistBeginAnimationTask() const;
	void BindToBeginAnimationFinished(const FOnLoadingWidgetAnimationDelegate& Task);

	bool IsExistEndAnimation() const;
	bool IsExistEndAnimationTask() const;
	void BindToEndAnimationFinished(const FOnLoadingWidgetAnimationDelegate& Task);

private:
	UFUNCTION()
	void OnBeginAnimationFinished();

	UFUNCTION()
	void OnEndAnimationFinished();

protected:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* BeginAnimation;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* EndAnimation;

private:
	TArray<FOnLoadingWidgetAnimationDelegate> BeginAnimationFinishedTasks;
	TArray<FOnLoadingWidgetAnimationDelegate> EndAnimationFinishedTasks;

};
