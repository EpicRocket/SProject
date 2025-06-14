// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "GameFramework/Info.h"
#include "Team/Interface/IGTeamAgent.h"

#include "GameplayPlayer.generated.h"

struct FGErrorInfo;
struct FGTeam;
class UGameplayTeamSubsystem;
class UGameplayPlayerComponent;

UCLASS(Abstract, BlueprintType, Blueprintable, Category = "Gameplay", ClassGroup = "Gameplay")
class GGAMECORE_API AGameplayPlayer : public AInfo, public IGTeamAgent
{
	GENERATED_BODY()

public:
	AGameplayPlayer();

	// IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~IGTeamAgent


public:
	UPROPERTY(EditInstanceOnly, Category = "Team")
	uint8 TeamID = 255;

};
