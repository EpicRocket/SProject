
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Attribute.h"
#include "Styling/SlateBrush.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Components/Widget.h"
#include "UObject/ScriptInterface.h"

#include "PaperFlipbookUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlipbookFinishedPlayingEvent);

class SPaperFlipbookWidget;
class UPaperFlipbook;

UCLASS(meta = (DisplayName = "Paper Flipbook"))
class GGAMECORE_API UPaperFlipbookUserWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Category = Appearance, EditAnywhere, meta = (DisplayThumbnail = "true"))
	UPaperFlipbook* SourceFlipbook;

	UPROPERTY(Category = Appearance, EditAnywhere)
	uint8 bAutoPlay : 1;

	UPROPERTY(Category = Appearance, EditAnywhere)
	float PlayRate;

	UPROPERTY(Category = Appearance, EditAnywhere)
	uint8 bLooping : 1;

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadOnly, meta = (sRGB = "true"))
	FLinearColor ColorAndOpacity;

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<enum ESlateBrushTileType::Type> Tiling;

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<enum ESlateBrushMirrorType::Type> Mirroring;

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadWrite)
	uint8 bOverrideContentSize : 1;

	UPROPERTY(Category = Appearance, EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideContentSize"))
	FVector2D DesiredSize;

	UPROPERTY()
	FGetLinearColor ColorAndOpacityDelegate;

public:
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnPointerEvent OnMouseButtonDownEvent;

public:
	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void SetOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void OverrideBrushSize(FVector2D InDesiredSize);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void SetBrushTintColor(FSlateColor TintColor);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void SetBrushTiling(TEnumAsByte<enum ESlateBrushTileType::Type> InTiling);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget|Appearance")
	void SetBrushMirroring(TEnumAsByte<enum ESlateBrushMirrorType::Type> InMirroring);

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

public:
	UPROPERTY(BlueprintAssignable, Category = "PaperFlipbook|Widget|Event")
	FOnFlipbookFinishedPlayingEvent OnFinishedPlaying;

public:
	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	virtual bool SetFlipbook(class UPaperFlipbook* NewFlipbook);

	UFUNCTION(BlueprintPure, Category = "PaperFlipbook|Widget")
	virtual UPaperFlipbook* GetFlipbook();

	/** Start playback of flipbook */
	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void Play();

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void PlayFromStart();

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void Reverse();

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void ReverseFromEnd();

	/** Stop playback of flipbook */
	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void Stop();

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	bool IsPlaying() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	bool IsReversing() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void SetPlaybackPositionInFrames(int32 NewFramePosition);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	int32 GetPlaybackPositionInFrames() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void SetPlaybackPosition(float NewPosition);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	float GetPlaybackPosition() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void SetLooping(bool bNewLooping);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	bool IsLooping() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void SetPlayRate(float NewRate);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	float GetPlayRate() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	void SetNewTime(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	float GetFlipbookLength() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	int32 GetFlipbookLengthInFrames() const;

	UFUNCTION(BlueprintCallable, Category = "PaperFlipbook|Widget")
	float GetFlipbookFramerate() const;

protected:
	TSharedPtr<SPaperFlipbookWidget> MyFlipbookWG;

protected:
	void OnFlipbookFinishedPlaying();

protected:
	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);
};
