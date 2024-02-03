

#include "SubUserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SubUserWidget)

USubUserWidget::USubUserWidget()
    : Super(FObjectInitializer::Get())
{
#if WITH_EDITOR
	PaletteCategory = FText::FromString(TEXT("Project Only"));
#endif
}
