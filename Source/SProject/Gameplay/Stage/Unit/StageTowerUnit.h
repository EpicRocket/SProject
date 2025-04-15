
#pragma once

#include "StageUnitCharacter.h"

#include "StageTowerUnit.generated.h"

struct FStageTowerInfo;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageTowerUnit : public AStageUnitCharacter
{
	GENERATED_BODY()

protected:
	virtual void InitailizeBaseStats() override;

public:
	void SetInfo(FStageTowerInfo NewInfo);
	
	UFUNCTION(BlueprintPure)
	FStageTowerInfo GetInfo() const;
	TSharedRef<FStageTowerInfo> GetInfoRef() const;

public:
	TSharedPtr<FStageTowerInfo> Info;
};
