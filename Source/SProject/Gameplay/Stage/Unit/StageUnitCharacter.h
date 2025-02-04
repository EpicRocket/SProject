
#pragma once

#include "Unit/UnitCharacter.h"

#include "StageUnitCharacter.generated.h"

UCLASS(Abstract)
class MY_API AStageUnitCharacter : public AUnitCharacter
{
	GENERATED_BODY()

public:
	AStageUnitCharacter();

private:
	UPROPERTY()
	TArray<TObjectPtr<class UGAttributeSet>> AttributeSetCache;
};
