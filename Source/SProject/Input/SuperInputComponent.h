// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "SuperInputComponent.generated.h"

class USuperInputConfig;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS()
class SPROJECT_API USuperInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	void AddInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const USuperInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
};
