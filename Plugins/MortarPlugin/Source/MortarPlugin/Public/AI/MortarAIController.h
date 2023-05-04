//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"	
#include "MortarAIController.generated.h"


/**
 * Predicate for Sorting
 * If distance of A with Owner is less than B then A will be choosen
 */
struct FMortarSortByDistance
{
	FMortarSortByDistance(const FVector& SourceLocation)
		                  : OwnerLocation(SourceLocation)
	{

	}

	/**
	 * The Location of Actor 
	 */
	FVector OwnerLocation;
   
	/**
	 *  Comparing Distances of both actors From the owner's location and returns true if
	 *  Actor A's distance from owner is less than B's distance from Owner's location
	 *  
	 *  @relatesalso AActor
	 */
	bool operator()(const AActor* A, const AActor* B) const
	{
		float ADistanceFromOwner = FVector::DistSquared(OwnerLocation, A->GetActorLocation());
		float BDistanceFromOwner = FVector::DistSquared(OwnerLocation, B->GetActorLocation());
		return ADistanceFromOwner < BDistanceFromOwner;
	}
};


/**
 * AI Controller Class . Mind Of AI
 */
UCLASS()
class MORTARPLUGIN_API AMortarAIController : public AAIController
{
	GENERATED_BODY()

public:

	/**
	 * Returns Behavior our AI Pawn towards a detected pawn
	 * @param[in] Other Detected Actor
	 * @return  Attitude of Current owner towards Other 
	 */
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:

	/**
	 *   This Returns Array which is sorted according to some condition could be distance etc
	 *   This Function is Blueprint Native Event Allowing User to override the functionality in Blueprints
	 *   @param[in] DetectedActors Tarray of Actors Detected by AI
	 *   @return TArray of Best Actors in Sorted Way
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Mortar|AI")
	TArray<AActor*> GetBestTarget(const TArray<AActor*>& DetectedActors);
	virtual TArray<AActor*> GetBestTarget_Implementation(const TArray<AActor*>& DetectedActors);

	/**
	 * AI Perception Event for detecting senses
	 */
	UPROPERTY()
	class UAIPerceptionComponent* MortarAIPerception;

	/**
	 * Blackboard to use Path Has to be setup in blueprint
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Mortar|AI")
	UBlackboardData* MortarBlackboard;

	/**
	 * Behavior Tree to run that will be setup in Blueprint
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Mortar|AI")
	UBehaviorTree* MortarBehaviorTree;

	/**
	 * Contains Blackboard all keys
	 */
	UPROPERTY()
	UBlackboardComponent* MortarBlackboardComponent;

private:

	/**
	 *  Function to be called on Perception Updation
	 *  @param[in] ActorsDetected Tarray of Actors Detected by AI
	 */
	UFUNCTION()
	virtual void SenseAllActors(const TArray<AActor*>& ActorsDetected);

	/**
	 * Event Called in beg
	 */
	virtual void BeginPlay() override;

	/**
	 * Sets Pawn and Register for events
	 * @param[in] InPawn Pawn to set
	 */
	virtual void SetPawn(APawn* InPawn) override;

	/**
	 * Event Called On Death. Here we generally like to disconnect
	 * Controller which then can be deleted
	 */
	UFUNCTION()
	void OnDeathDelegate();

	/**
	 * Event Called When AI Dies and in future would want to resume
	 *
	 */
	UFUNCTION()
	void OnPauseDelegate();

	/**
	 * Event Called When AI Respawns after Death
	 */
	UFUNCTION()
	void OnResumeDelegate();

	/**
	 * Pointer To actors Object containing detected actors to be used in blackboard
	 */
	UPROPERTY(Transient)
	class UMortarActorsObject* ActorsObject;

	/**
	 *  Team ID of the owner of this AI Controller
	 */
	FGenericTeamId OwnerTeamID;

	/**
	 * Key Name That Will be set in BehaviorTree By this keyname ai controller will set the values which will be later used
	 * Change Carefully need to change in behavior tree also
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Mortar|AI")
	FName EnemyKey = "DetectedEnemies";

};
