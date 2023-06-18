
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnitAnimInstance.generated.h"

UCLASS()
class SPROJECT_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe, AnimBlueprintFunction))
	bool IsWalk() const;
};
