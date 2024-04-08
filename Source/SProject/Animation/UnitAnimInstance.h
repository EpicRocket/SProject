
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "UnitAnimInstance.generated.h"

UCLASS()
class UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void InitializeWithAbilitySystem(class UAbilitySystemComponent* ASC);

protected:
	virtual void NativeInitializeAnimation() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
