
#pragma once

#include "CommonButtonBase.h"

#include "TextButtonBase.generated.h"

class UCommonTextBlock;
namespace ETextJustify
{
	enum Type : int;
}

UCLASS(Abstract, BlueprintType, Blueprintable)
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
