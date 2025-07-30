// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "AttributeSet.h"

#include "GAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GGAMECORE_API UGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
};
