
#pragma once

#include "StageUnitCharacter.h"

#include "StageTowerUnit.generated.h"

struct FStageTowerInfo;
class UStageTowerContext;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageTowerUnit : public AStageUnitCharacter
{
	GENERATED_BODY()

protected:
	// StageUnitCharacter
	virtual void OnInit() override;
	virtual void OnInitBaseStats() override;
	// ~StageUnitCharacter

public:
	void Setup(FStageTowerInfo NewTowerInfo);
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetTowerInfo", ReturnDisplayName = "Error"))
	FGErrorInfo K2_GetTowerInfo(FStageTowerInfo& TowerInfo);
	TOptional<FStageTowerInfo> GetTowerInfo() const;

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetSellPrice", ReturnDisplayName = "Error"))
	FGErrorInfo K2_GetSellPrice(int32& SellPrice);
	TOptional<int32> GetSellPrice() const;

public:
	UPROPERTY(Transient)
	TObjectPtr<UStageTowerContext> TowerInfoContext;

};
