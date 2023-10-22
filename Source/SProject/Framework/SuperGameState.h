
#pragma once

#include "ModularGameState.h"
#include "AbilitySystemInterface.h"

#include "SuperGameState.generated.h"

class USuperAbilitySystemComponent;

UCLASS()
class ASuperGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASuperGameState(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USuperAbilitySystemComponent> AbilitySystemComponent;
};
