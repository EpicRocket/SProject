// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "AIController.h"

#include "StageAIController.generated.h"

class AStageLevel;
class UBehaviorTree;

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageAIController : public AAIController
{
	GENERATED_BODY()

protected:
	// IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	// ~IGenericTeamAgentInterface

public:
	UFUNCTION(BlueprintCallable)
	void Setup(uint8 InTeamID, UBehaviorTree* InAI);

	UFUNCTION(BlueprintCallable)
	void StartAI();

	UFUNCTION(BlueprintCallable)
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Stage")
	TWeakObjectPtr<AStageLevel> SourceStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> AIBehaviorTree;

};
