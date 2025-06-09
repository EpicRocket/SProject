// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CommonButtonBase.h"

#include "TextButtonBase.generated.h"

class UCommonTextBlock;
namespace ETextJustify
{
	enum Type : int;
}

UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, ClassGroup = "UI", meta = (Category = "GameCore UI", DisableNativeTick, DontUseGenericSpawnObject = "true"))
class GGAMECORE_API UTextButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties() override;

	virtual void NativeOnCurrentTextStyleChanged() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Text")
	void SetText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Text")
	void SetJustification(ETextJustify::Type InJustification);

private:
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* ContentText;

	UPROPERTY(EditAnywhere, Category = "Text")
	FText Text;
};
