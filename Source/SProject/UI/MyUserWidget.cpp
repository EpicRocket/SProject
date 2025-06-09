// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "MyUserWidget.h"

#if WITH_EDITOR
const FText UMyUserWidget::GetPaletteCategory()
{
	return FText::FromString(TEXT("My UI"));
}
#endif
