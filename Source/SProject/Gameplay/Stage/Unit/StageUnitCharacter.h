
#pragma once

#include "Unit/UnitCharacter.h"
#include "Gameplay/Stage/Unit/IStageUnitAnimMontage.h"

#include "StageUnitCharacter.generated.h"

enum class EStageUnitAttribute : uint8;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageUnitCharacter : public AUnitCharacter, public IStageUnitAnimMontage
{
	GENERATED_BODY()

public:
	AStageUnitCharacter();

	void virtual BeginPlay() override;

	// IStageUnitAnimMontage
	virtual UAnimMontage* GetAnimMontage(UPARAM(meta = (Categories = "AnimState")) FGameplayTag AnimState) const;
	// ~IStageUnitAnimMontage

public:
	UFUNCTION(BlueprintPure)
	const class UStageUnitAttributeSet* GetUnitSet() const;

protected:
	virtual void InitailizeBaseStats() {}

	void SetBaseStats(TMap<EStageUnitAttribute, double> Params);

	UPROPERTY(EditDefaultsOnly, Category = "Unit")
	TObjectPtr<class UGameplayUnitAnimStateDataAsset> AnimStateDataAsset;

private:
	UPROPERTY()
	TArray<TObjectPtr<class UGAttributeSet>> AttributeSetCache;

};
