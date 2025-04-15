
#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "StageDamageExecutionCalculation.generated.h"

UCLASS()
class MY_API UStageDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()


protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
