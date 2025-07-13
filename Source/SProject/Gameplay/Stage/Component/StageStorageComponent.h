
#pragma once

#include "Core/Component/GPlayerComponent.h"

#include "StageStorageComponent.generated.h"

struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UStageStorageComponent : public UGPlayerComponent
{
	GENERATED_BODY()

protected:
	// UActorComponent
	virtual void InitializeComponent() override;
	// ~UActorComponent

public:
	UFUNCTION(BlueprintPure)
	int32 GetLastStageLevel() const;

	TSharedPtr<FStage> GetStage(int32 InStageLevel) const;

	void SetStage(const FStage& InStage);

	void Flush();
	// 이 컴포넌트가 가지고 있는 데이터는 임시 데이터로 게임 플레이 중의 스테이지에 대한 정보를 기록하고 갱신 할 수 있어야 합니다.

private:
	mutable int32 LastStageLevel = INDEX_NONE;

	mutable TMap<int32/*Level*/, TSharedPtr<FStage>> Stages;

};
