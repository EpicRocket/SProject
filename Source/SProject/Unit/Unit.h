

#pragma once

#include "GameFramework/Character.h"
#include "Unit.generated.h"

class UMyAbilitySystemComponent;
class UUnitAttributeSet;

UCLASS(Abstract, ClassGroup = Unit, Category = Unit)
class AUnit : public ACharacter
{
	GENERATED_BODY()

	static FName ASCName;
	static FName UnitSetName;

public:
	AUnit();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGuid UnitId;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UUnitAttributeSet> UnitSet;
};
