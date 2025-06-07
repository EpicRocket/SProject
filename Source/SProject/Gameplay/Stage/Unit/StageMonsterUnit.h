// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "StageUnitCharacter.h"

#include "StageMonsterUnit.generated.h"

struct FStageMonsterInfo;
class UStageMonsterContext;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageMonsterUnit : public AStageUnitCharacter
{
	GENERATED_BODY()

protected:
	// StageUnitCharacter
	virtual void OnInit() override;
	virtual void OnInitBaseStats() override;
	// ~StageUnitCharacter

public:
	void Setup(FStageMonsterInfo NewMonsterInfo);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetMonsterInfo", ReturnDisplayName = "Find"))
	FGErrorInfo K2_GetMonsterInfo(FStageMonsterInfo& MonsterInfo);
	TOptional<FStageMonsterInfo> GetMonsterInfo() const;

public:
	UPROPERTY(Transient)
	TObjectPtr<UStageMonsterContext> MonsterInfoContext;

};
