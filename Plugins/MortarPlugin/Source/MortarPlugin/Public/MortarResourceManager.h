//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MortarResourceManager.generated.h"


/*
 * This class is responisble For Managing the total number of actors
 * that can be there in a single level. If the limit exceeds it will simply
 * delete the extra actors to save performance.This is an abstract class
 * so this won't be spawned rather the blueprint which is deriving from
 * will get spawned
 */
UCLASS(Abstract,Blueprintable)
class MORTARPLUGIN_API UMortarResourceManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/*
	 *  Adds an actor in the Map for later reference
	 *  If the size exceeds the maximum limit it will delete 10 actors from pool
	 *  and then add
	 *  param[in]  Actor To add in The pool
	 *  param[in]  Subclass of the actor
	 */
	void AddToPool(class AActor* ActorToAdd, TSubclassOf<AActor> SubClass);

	/*
	 *  Removes an actor From The Tarray. Note It Doesn't Destory The Actor
	 *  param[in]  Actor To Remove from The pool
	 *  param[in]  Subclass of the actor
	 *  return     True if Actor Found in the pool
	 *             False Otherwise
	 */
	bool RemoveFromPool(class AActor* ActorToRemove, TSubclassOf<AActor> SubClass);

	/**
	 * Event Called on Initializing subsystem which will be handled in blueprint
	 * The Blueprint should specify the actors and the number of maximum actors
	 * that can be spawned
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortar|ResourceManager")
	void InitializeResourceManagement();

private:

	/**
	 *   Initializes Subsystem
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/*
	 *	 Tells UE4 whether this subsystem is to be loaded for all
	 *   @return  True means initialize in all 
	 *            False means manually initialized
	 */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const { return true; }

	/*
	 *  Sets Resource Count For Each Type Of Actor We gonna see it
	 *  @param[in]  ResourceCount Map Of Type of Actor and the total number of resources we gonna see in 
	 *              a level
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|ResourceManager")
	void SetResourceCount(const TMap<TSubclassOf<class AActor>, int32>& ResourceCount);

    /**
	 *  Map Containing Per Resource Max Count
	 */
	TMap<TSubclassOf<class AActor>, int32> PerResourceCount;

	/**
	 *  Map Containing Tarray of Actors For each type of actor Specified
	 *  This is a pool Which stores all the actors used
	 */
	TMap<TSubclassOf<AActor>, TArray<AActor*>> ResourcePool;

	/**
	 * Max No of Actors To destory when limit exceeds. By Default 10
	 */
	int32 MaxOneTimeDelete;

	/**
	 * Mutex for protecting TArray 
	 */
	FCriticalSection ArrayProtectorMutex;

};
