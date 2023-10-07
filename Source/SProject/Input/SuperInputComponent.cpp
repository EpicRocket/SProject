// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SuperInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "System/SUPERGameUserSettings.h"
#include "System/SUPERMappableConfigPair.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SuperInputComponent)

void USuperInputComponent::AddInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void USuperInputComponent::RemoveInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void USuperInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
