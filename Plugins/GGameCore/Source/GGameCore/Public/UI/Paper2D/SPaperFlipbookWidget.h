
#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SLeafWidget.h"

class FPaintArgs;
class FSlateWindowElementList;
class UPaperFlipbook;
class UPaperSprite;

class GGAMECORE_API SPaperFlipbookWidget : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SPaperFlipbookWidget)
		: _ColorAndOpacity(FLinearColor::White)
		, _OnMouseButtonDown()
	{
	}

	SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)

		SLATE_EVENT(FSimpleDelegate, OnFinishedPlaying)

		SLATE_END_ARGS()

	SPaperFlipbookWidget();

	void Construct(const FArguments& InArgs);

	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);

	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	void SetOnMouseButtonDown(FPointerEventHandler EventHandler);

	void SetOnFinishedPlaying(FSimpleDelegate InOnFinishedPlaying);

	void OverrideBrushSize(FVector2D InDesiredSize, bool bOverride);

	void SetBrushTintColor(FSlateColor TintColor);

	void SetBrushTiling(TEnumAsByte<enum ESlateBrushTileType::Type> InTiling);

	void SetBrushMirroring(TEnumAsByte<enum ESlateBrushMirrorType::Type> InMirroring);

	FSlateBrush* GetBrush();

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual bool SetFlipbook(class UPaperFlipbook* NewFlipbook);

protected:
	virtual FVector2D ComputeDesiredSize(float) const override;

public:
	virtual UPaperFlipbook* GetFlipbook();

	void Play();

	void PlayFromStart();

	void Reverse();

	void ReverseFromEnd();

	void Stop();

	bool IsPlaying() const;

	bool IsReversing() const;

	void SetPlaybackPositionInFrames(int32 NewFramePosition, bool bFireEvents);

	int32 GetPlaybackPositionInFrames() const;

	void SetPlaybackPosition(float NewPosition, bool bFireEvents);

	float GetPlaybackPosition() const;

	void SetLooping(bool bNewLooping);

	bool IsLooping() const;

	void SetPlayRate(float NewRate);

	float GetPlayRate() const;

	void SetNewTime(float NewTime);

	float GetFlipbookLength() const;

	int32 GetFlipbookLengthInFrames() const;

	float GetFlipbookFramerate() const;

protected:
	void CalculateCurrentFrame();

	UPaperSprite* GetSpriteAtCachedIndex() const;

	void TickFlipbook(float DeltaTime);

protected:
	FSlateBrush DefaultBrush;

	uint8 bUseSpriteSize : 1;

	TAttribute<FSlateColor> ColorAndOpacity;

	FPointerEventHandler OnMouseButtonDownHandler;

	FSimpleDelegate OnFinishedPlaying;

	UPaperFlipbook* SourceFlipbook;

	float PlayRate;

	uint8 bLooping : 1;

	uint8 bReversePlayback : 1;

	uint8 bPlaying : 1;

	float AccumulatedTime;

	int32 CachedFrameIndex;
};
