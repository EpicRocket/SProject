//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Perception/AISightTargetInterface.h"
#include "MortarResourceManager.h"
#include "Mortar.generated.h"




/**
 * Event Called On Death. Here we generally like to disconnect
 * Controller which then can be deleted
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarDeathEvent);

/**
 * Event Called When Mortar Dies and in future would want to resume
 * Both Player and AI controller would want to pause their's controllers
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarPauseEvent);

/**
 * Event Called When Mortar Respawns
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarResumeEvent);


/**
 * Main Actor Class for Mortar Project.This will contain three main components,
 * Static Base Then Turret 3 Then Mortar Gun/Barrel
 */
UCLASS()
class MORTARPLUGIN_API AMortar : public APawn, public IAISightTargetInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this pawn's properties
	AMortar();

	/**
	 *   Event Called After Death. Blueprint event only Can Add Animation sound etc
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning", meta = (DisplayName = "After Death Event"))
	void ActorDeathEvent();

	/**
	 *   Event Called Before Respawning of Actor after Death. Blueprint event only Can Add sounds,effets
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning", meta = (DisplayName = "On Start Respawn Event"))
	void OnActorStartRespawnEvent();

	/**
	 *   Event Called Before Respawning of Actor has been completed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning", meta = (DisplayName = "On End Respawn Event"))
	void OnActorEndRespawnEvent();

	/**
	 *   Blue Print Event Called After Team Change
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Team", meta = (DisplayName = "On Team Change Event"))
	void ActorTeamChangeEvent();

	/**
	 *   Function to Set Time After which mortar actor will be destroyed from world
	 *   @param[in] DestroyDelay Time After Which to Destroy
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetDestroyMortarDelay(const float& DestroyDelay);

	/**
	 *   Function to Set Unlimited Spawning
	 *   @param[in] UnlimitedRespawning True to Enable Unlimited Spawning False otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetUnlimitedRespawn(const bool& UnlimitedRespawning);

	/**
	 *   Function to How Many Times Spawning Is Allowed.Won't have any effect if Unlimited Spawning is on
	 *   @param[in] Count Total Times to Spawn
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetRespawnCount(const uint8& Count);

	/**
     *   New Function This function is sets the subsystem for resourceManagement
     *   @param[in] resourceManager Takes a Class Name of ResourceManager
     */
	UFUNCTION(BlueprintCallable, Category = "ResourceManagmenet")
	void SetResourceManager(UMortarResourceManager* resourceManager);

	/**
	 *   Getter Function To get Delay after which mortar will be dstroyed after spawning limit has been exhausted
	 *   @return Time Delay in float
	 */
	UFUNCTION(BlueprintPure , Category = "Spawning")
	float GetDestroyMortarDelay() const;

	/**
	 *   Function to Get whether the mortar has infinite life( To infinity and beyond :) )
	 *   @return Bool variable true if unlimited spawning false otherwise
	 */
	UFUNCTION(BlueprintPure, Category = "Spawning")
	bool IsUnlimitedRespawnable() const;

	/**
	 *   Getter function to tell How Many Times Spawning Is Allowed.
	 *   @return unsigned bytes variable containing lives left
	 */
	UFUNCTION(BlueprintPure, Category = "Spawning")
	uint8 GetRespawnCount() const;

	/**
	 *   Death Event That Will be Broadcasted as soon as Health Reaches 0 and Respawning Limit has been reached
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarDeathEvent OnMortarDeath;

	/**
	 *   Pause Event that will be broadcasted as soon as Health Reaches 0 like disabling AI camera etc
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarPauseEvent OnMortarPause;

	/**
	 *   Resume Event that will be broadcasted after AI respawns
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarResumeEvent OnMortarResume;

protected:

	/**
	 *   Function to Override to allow seeing from Mortar Gun i.e Barrel
	 *   More Info @see https://docs.unrealengine.com/en-US/API/Runtime/AIModule/Perception/IAISightTargetInterface/CanBeSeenFrom/index.html
	 */
	virtual bool CanBeSeenFrom
	(
		const FVector& ObserverLocation,
		FVector& OutSeenLocation,
		int32& NumberOfLoSChecksPerformed,
		float& OutSightStrength,
		const AActor* IgnoreActor,
		const bool* bWasVisible = nullptr,
		int32* UserData = nullptr
	) const;

	/**
	 * Function to afflict damage This will be called whenever the actor Receives damage
	 * @param[in] DamageAmount  Damage Amount
	 * @param[in] DamageEvent  Data package that fully describes the damage received.
	 * @param[in] EventInstigator  The Controller responsible for the damage.
	 * @param[in] DamageCauser Damage Causer Here In our example it will be Projectile Actor
	 */
	virtual float TakeDamage
	(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class  AController* EventInstigator,
		AActor* DamageCauser
	) override;

	/**
	 *Called when the game starts or when spawned
	 */
	virtual void BeginPlay() override;

	/**
	 *  Health Bar Widget to be Configured in BP
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UWidgetComponent* HealthBar = nullptr;

	/**
	 * Base Structure Mesh of the Mortar This is immovable object
	 */
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category = "Setup")
	class UStaticMeshComponent* MortarBase = nullptr;

	/**
	 * Static Mesh that moves Right to Left
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setup")
	class UMortarTurret* MortarTurret = nullptr;

	/**
	 * Gun Moves Up and Down
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setup")
	class UMortarBarrel* MortarGun = nullptr;

	/**
	 * Generic Health Component That will handle all the health related things.Can Be added to any actor
	 */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Setup")
	class UMortarHealthComponent* MortarHealthComponent = nullptr;

	/**
	 * Aiming Component Which Will handle all the aiming and shooting
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setup")
	class UMortarAimingComponent* MortarAimingComponent = nullptr;

	/**
	 * Generic Team Component Telling which actor belongs to which team
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "Setup")
	class UMortarTeamComponent* MortarTeamComponent = nullptr;

	/**
	 * Noise Emitter For Pawn Hearing Sensing
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	class UPawnNoiseEmitterComponent* MortarNoiseEmitter = nullptr;

private:

	/**
	 * Destroy Function Which will be called after death
	 * This will be called after the Destroy delay
	 */
	void MortarDestroy();

	/**
	 * Respawn Function Which will be called after death.Here Mainly we want
	 * to play respawn effects and sound
	 * This will be called after the respawn delay
	 */
	void StartMortarRespawn();

	/**
	 * Respawn Function Which after playing effect we want to resume firing
	 */
	void AfterMortarRespawn();

	/**
	 * Delegate For On Team Change Event
	 */
	UFUNCTION()
	void OnTeamChangeDelegate();

	/**
	 * Time after which Mortar will be deleted
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Spawning")
	float DestroyMortarDelay = 5.f;

	/**
	 * Time after which Mortar will be deleted
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Spawning")
	float RespawnShootingDelay = 2.f;
	
	/**
	 * Allow Unlimited Respawning of Mortar Actor.Each Time Mortar Dies then it will respawn after
	 * the mentioned delay with default attributes
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Spawning")
	bool bUnlimitedRespawn = false;

	/**
	 * How Many Times Respawning is Allowed. Only Works if Unlimited Respawning if Off
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Spawning", meta = (EditCondition = "!bUnlimitedRespawn"))
	uint8 RespawnCount = 0;

	/**
	 * Destruction In Progress
	 */
	uint32 DestroyInProgress : 1;

	/**
	 * Respawning In Progress
	 */
	uint32 RespawnInProgress : 1;

	/**
	 * Reference FOr REsource Manager. This needs to be set once only by anyone
	 * Used only for reference we wont be doing anything with this
	 */
	class UMortarResourceManager* ResourceManager;

};
