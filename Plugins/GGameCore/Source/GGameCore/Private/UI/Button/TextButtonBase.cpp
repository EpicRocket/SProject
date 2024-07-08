
#include "UI/Button/TextButtonBase.h"
#include "Framework/Text/TextLayout.h"
#include "CommonTextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TextButtonBase)

void UTextButtonBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetText(Text);
}

void UTextButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (IsValid(ContentText))
	{
		ContentText->SetStyle(GetCurrentTextStyleClass());
	}
}

void UTextButtonBase::SetText(const FText& InText)
{
	if (IsValid(ContentText))
	{
		ContentText->SetText(InText);
	}
}

void UTextButtonBase::SetJustification(ETextJustify::Type InJustification)
{
    if (IsValid(ContentText))
    {
		ContentText->SetJustification(InJustification);
    }
}
