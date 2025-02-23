
#pragma once

#include "Core/Component/GPlayerComponent.h"

#include "StageStorageComponent.generated.h"

struct FLatentActionInfo;
struct FGErrorInfo;
struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UStageStorageComponent : public UGPlayerComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo WaitForInitialize(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure)
	FStage GetLastStage() const;

private:
	int32 LastStageLevel = INDEX_NONE;

	TMap<int32/*Level*/, TSharedPtr<FStage>> Stages;

};
