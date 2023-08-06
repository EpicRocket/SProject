// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SuperInputConfig.h"

const UInputAction* USuperInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FSuperTagedInputAction& TaggedInputAction : NativeInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* USuperInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return nullptr;
}
