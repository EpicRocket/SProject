
#pragma once

#include "ModularGameState.h"
#include "AbilitySystemInterface.h"

#include "SuperGameState.generated.h"

class USuperAbilitySystemComponent;
class AUnit;

UCLASS()
class ASuperGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASuperGameState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual AUnit* SpawnUnit(const FVector& Location, const FRotator& Rotation, TSubclassOf<AUnit> UnitClass);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USuperAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TMap<FGuid, AUnit*> CacheUnitMap;
};
