
#pragma once

#include "Framework/Player/GPlayerComponent.h"

#include "StageStorageComponent.generated.h"

struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UStageStorageComponent : public UGPlayerComponent
{
	GENERATED_BODY()

public:
	

	UFUNCTION(BlueprintPure)
	FStage GetLastStage() const;

private:
	TMap<int32/*Level*/, TSharedPtr<FStage>> Stages;

};
