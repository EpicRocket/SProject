

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "MyHealExecution.generated.h"

class UObject;


/**
 * UMyHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class UMyHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UMyHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
