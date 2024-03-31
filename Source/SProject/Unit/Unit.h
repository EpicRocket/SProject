

#pragma once

#include "GameFramework/Character.h"
// #include "GameplayCueInterface.h"
#include "Unit.generated.h"

class UMyAbilitySystemComponent;

UCLASS(Abstract, ClassGroup = Unit, Category = Unit)
class AUnit : public ACharacter
{
	GENERATED_BODY()

public:
	AUnit();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGuid UnitId;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;
};
