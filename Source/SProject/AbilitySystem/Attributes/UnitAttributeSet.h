
#pragma once

#include "MyAttributeSet.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"

#include "UnitAttributeSet.generated.h"

#define UNIT_ATTRIBUTE_ACCESSORS(PropertyName) ATTRIBUTE_ACCESSORS(UUnitAttributeSet, PropertyName)

UCLASS(BlueprintType, ClassGroup = Unit, Category = AbilityAttribute)
class UUnitAttributeSet : public UMyAttributeSet
{
    GENERATED_BODY()

public:
    UNIT_ATTRIBUTE_ACCESSORS(Life);
    UNIT_ATTRIBUTE_ACCESSORS(Damage);
    UNIT_ATTRIBUTE_ACCESSORS(Armor);


private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Life = 1.0F;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Damage = 0.0F;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Armor = 0.0F;
};
