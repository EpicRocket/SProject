

#pragma once

#include "AbilitySystemComponent.h"
#include "MyAttributeSet.h"

#include "MyCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * UMyCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class UMyCombatSet : public UMyAttributeSet
{
	GENERATED_BODY()

public:

	UMyCombatSet();

	ATTRIBUTE_ACCESSORS(UMyCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UMyCombatSet, BaseHeal);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, Category = "My|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, Category = "My|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
