
#pragma once

#include "Components/GameStateComponent.h"

#include "StageStorageComponent.generated.h"

struct FStageDetail;

UCLASS()
class MY_API UStageStorageComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:


private:
	TWeakObjectPtr<FStageDetail> LastStageDetail;

	TMap<int32/*Level*/, TSharedPtr<FStageDetail>> StageDetails;

};
