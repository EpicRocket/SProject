// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Team/Interface/IGTeamAgent.h"

#include "GPlayerController.generated.h"

class APawn;

UCLASS(BlueprintType, Blueprintable, Category = "Core", ClassGroup = "Core")
class GGAMECORE_API AGPlayerController : public APlayerController, public IGTeamAgent
{
	GENERATED_BODY()

public:
	// APlayerController
	virtual void ReceivedPlayer() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnUnPossess() override;
	// ~APlayerController

protected:
	// AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~AActor

public:
	// IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~IGTeamAgent

protected:
	UPROPERTY(EditInstanceOnly, Category = "Team")
	uint8 TeamID = 255;
};
