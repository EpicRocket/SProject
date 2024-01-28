

#include "CustomUserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomUserWidget)

UCustomUserWidget::UCustomUserWidget()
    : Super(FObjectInitializer::Get())
{
#if WITH_EDITOR
	PaletteCategory = FText::FromString(TEXT("Project Only"));
#endif
}
