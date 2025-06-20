// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Loading/UI/GLoadingWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GLoadingWidget)

#if WITH_EDITOR
const FText UGLoadingWidget::GetPaletteCategory()
{
	return FText::FromString(TEXT("Loading"));
}
#endif

void UGLoadingWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

}
void UGLoadingWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

}
