//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MortarAIBaseTaskNode.generated.h"

/**
 * Bass Class For All AI Behavior Tasks.Class Containts Validation
 * logic for the detected enemies
 */
UCLASS()
class MORTARPLUGIN_API UMortarAIBaseTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

protected:

   /**  Chooses and REturns Best Target
	*   If No Actor Fullfills the conditions then nullptr would be returned
	*   and should be handled in child classes accordingly
	*   @param[in] KeyName Key from where to extract Detected Actors from AI Controller
	*   @param[in] BlackboardComp Blackboard Component
	*   @param[in] AIController AI Controller of the pawn
	*   @return Best Actor .This will be the main target for the ai
	*/
	virtual AActor* GetBestTarget(const FName& KeyName, UBlackboardComponent* BlackboardComp, AAIController* AIController);

private:

	/** Whether to Select Enemies i.e with diff team id . Detection by Affilation should be configured
	 *   accordingly in AI Controller
	 *
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|AI")
	bool bSelectEnemy = true;

	/** Whether to Select Members of same team id . Detection by Affilation should be configured
	 *  accordingly in AI Controller
	 *
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|AI")
	bool bSelectFriends = false;

	/** Whether to Select Neutral Members i.e. of team id = 0. Detection by Affilation should be configured
	 *  accordingly in AI Controller
	 *
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|AI")
	bool bSelectNeutrals = false;
};
