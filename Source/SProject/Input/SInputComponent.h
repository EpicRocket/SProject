// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SInputComponent.generated.h"

class USInputConfig;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS()
class SPROJECT_API USInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	void AddInputMappings(const USInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const USInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
};
