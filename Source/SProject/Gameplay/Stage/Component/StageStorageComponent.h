
#pragma once

#include "Gameplay/Component/GameplayPlayerComponent.h"

#include "StageStorageComponent.generated.h"

struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UStageStorageComponent : public UGameplayPlayerComponent
{
	GENERATED_BODY()

protected:
	// UActorComponent
	virtual void InitializeComponent() override;
	// ~UActorComponent

public:
	UFUNCTION(BlueprintPure)
	FStage GetLastStage() const;

private:
	int32 LastStageLevel = INDEX_NONE;

	TMap<int32/*Level*/, TSharedPtr<FStage>> Stages;

};
