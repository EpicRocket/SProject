#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attribute/GAttributeSet.h"

#include "GBaseAttributeSet.generated.h"

class UObject;

UCLASS(BlueprintType)
class UGBaseAttributeSet : public UGAttributeSet
{
	GENERATED_BODY()

public:
	UGBaseAttributeSet();

	ATTRIBUTE_ACCESSORS(UGBaseAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UGBaseAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UGBaseAttributeSet, Damage);

	mutable FGAttributeEvent OnHealthChanged;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "GameCore|BaseAttribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	bool bOutOfHealth;

	UPROPERTY(BlueprintReadOnly, Category = "GameCore|BaseAttribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "GameCore|BaseAttribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};

