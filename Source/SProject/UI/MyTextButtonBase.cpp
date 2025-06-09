// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "MyTextButtonBase.h"

#if WITH_EDITOR
const FText UMyTextButtonBase::GetPaletteCategory()
{
    return FText::FromString(TEXT("My UI"));
}
#endif
