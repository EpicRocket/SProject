

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "MyDamageExecution.generated.h"

class UObject;


/**
 * UMyDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UMyDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UMyDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
