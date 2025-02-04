
#pragma once

#include "Unit/UnitCharacter.h"
#include "StageUnitCharacter.h"
#include "Gameplay/Stage/Interface/IStageTower.h"

#include "UnitStageTower.generated.h"

struct FBuildStageTower;

UCLASS()
class MY_API AUnitStageTower : public AStageUnitCharacter, public IStageTower
{
	GENERATED_BODY()

public:
	// IStageTower
	virtual EStageTowerType GetTowerType() const;
	virtual int32 GetKind() const;
	virtual int32 GetLevel() const;
	virtual int64 GetSellPrice() const;
	// ~IStageTower

	// FIXME: 이거 어떻게 할 수 있는 방법이 없을까? 스폰하는 곳에서 순수하게 설정할 수 있도록 하고 싶은데
	void SetBuildReceipt(const FBuildStageTower& Tower);

protected:
	TSharedRef<FBuildStageTower> GetBuildReceipt() const;

private:
	mutable TSharedPtr<FBuildStageTower> BuildReceipt;
};
