

#pragma once

#include "GameFramework/Character.h"
#include "UnitCharacter.generated.h"

class UMyAbilitySystemComponent;
class UUnitAttributeSet;

UCLASS(Abstract, ClassGroup = "Unit", Category = "Unit")
class MY_API AUnitCharacter : public ACharacter
{
	GENERATED_BODY()

	static FName ASCName;
	static FName UnitSetName;

public:
	AUnitCharacter();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGuid UnitId;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UUnitAttributeSet> UnitSet;
};
