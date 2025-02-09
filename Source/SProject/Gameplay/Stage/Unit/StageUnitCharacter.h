
#pragma once

#include "Unit/UnitCharacter.h"

#include "StageUnitCharacter.generated.h"

enum class EStageUnitAttribute : uint8;
class UStageUnitAttributeSet;

UCLASS(Abstract)
class MY_API AStageUnitCharacter : public AUnitCharacter
{
	GENERATED_BODY()

public:
	AStageUnitCharacter();

	void virtual BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	const UStageUnitAttributeSet* GetUnitSet() const;

protected:
	virtual void InitailizeBaseStats() {}

	void SetBaseStats(TMap<EStageUnitAttribute, double> Params);

private:
	UPROPERTY()
	TArray<TObjectPtr<class UGAttributeSet>> AttributeSetCache;
};
