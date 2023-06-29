// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SInputConfig.h"

const UInputAction* USInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FSTagedInputAction& TaggedInputAction : NativeInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* USInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	return nullptr;
}
