// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "MyButtonBase.h"

#if WITH_EDITOR
const FText UMyButtonBase::GetPaletteCategory()
{
    return FText::FromString(TEXT("My UI"));
}
#endif
