// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "MyActivatableWidget.h"

#if WITH_EDITOR
const FText UMyActivatableWidget::GetPaletteCategory()
{
    return FText::FromString(TEXT("My UI"));
}
#endif
