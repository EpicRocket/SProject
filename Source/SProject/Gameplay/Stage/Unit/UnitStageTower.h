
#pragma once

#include "Unit/UnitCharacter.h"
#include "Gameplay/Stage/Interface/IStageTower.h"

#include "UnitStageTower.generated.h"

struct FBuildStageTower;

UCLASS()
class MY_API AUnitStageTower : public AUnitCharacter, public IStageTower
{
    GENERATED_BODY()

public:
    // IStageTower
    virtual EStageTowerType GetTowerType() const;
	virtual int32 GetKind() const;
	virtual int32 GetLevel() const;
	virtual int64 GetSellPrice() const;
    // ~IStageTower

protected:
	TSharedRef<FBuildStageTower> GetBuildReceipt() const;

private:
    mutable TSharedPtr<FBuildStageTower> BuildReceipt;
};
