//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MortarAimingComponent.generated.h"


/**
 * Event Called When Mortar Fires Projectile
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarFireEvent);

/**
* Enum for firing State
* this enables UE4 header tool to know its enum
* and if wanted we can display in editor
*/
UENUM()
enum class EMortarFiringState :uint8
{
	Reloading, ///< Turret Is Reloading 
	Aiming, ///< Turret Moving and aiming 
	Locked, ///< Turret Fire Ready.
	OutOfAmmo ///< Out Of Ammo
};

/**
 *  Mortar Aiming Component All Rotation ,Aiming And Firing Will be controlled by this class
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MORTARPLUGIN_API UMortarAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UMortarAimingComponent();

	/**
	 * Checks if Mortar is able to aim and then moves mortar at the Desired Location
	 * @param[in] AimLocation Desired Aiming Location in FVector
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void AimAt(const FVector& AimLocation);

	/**
	 * Spawns A projectile at the desired socket location after checking mortar is in locked state
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void Fire();

	/**
	 * Initializes Turret and Barrel
	 * @param[in] TurretToSet Turret That will be used while Aiming
	 * @param[in] BarrelToSet Barrel that will be used while aiming
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void InitializeAiming(class UMortarTurret* TurretToSet, UMortarBarrel* BarrelToSet);

	/**
	 * Gets The Current State of Gun
	 * @return Enum of Firing State which can be Reloading,Aiming,Locked,OutOfAmmo
	 */
	EMortarFiringState GetCurrentState() const;

	/**
	 * Gets The Current Firing Count Left
	 * @return Firing Count Left
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	int32 GetFiringCount() const;

	/**
	 * Resets Firing Parameters like count,speed etc can be used after powerup
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void ResetFiring();

	/**
	 * Sets Time Delay between each shots
	 * @param[in] ReloadTime Float variable containing reload Time
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void SetReloadTimeInSeconds(const float& ReloadTime);

	/**
	 * Sets Firing Left
	 * @param[in] FiringLeft Firing Count Left
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void SetFiringLeft(const int32& FiringLeft);

	/**
	 * Sets Shots Spread Range
	 * @param[in] ShotsSpread Shots Spread In Degree
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	void SetShotsSpreadDegree(const float& ShotsSpread);
	
	/**
	 * Gets Reload Time for The Mortar Gun i.e Time Delay between each shot
	 * @return Float variable containing reload time in seconds
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Firing")
	int32 GetReloadTimeInSeconds() const;

	/**
	 * Provides information whether there is limit on ammo
	 * @return True if unlimited ammo that is no limit and false if limited ammo
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Firing")
	bool HasUnlimitedAmmo() const;

	/**
	 * returns true if the barrel is moving
	 * compares the barrel/Gun forward vector
	 * @return True if Barrel is moving false otherwise
	 */
	bool IsBarrelMoving();

	/**
	 *  Gets Shots Spread Range which is used while Firing.
	 *  @return Float Variable Containing Shot spread in degree
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Firing")
	float GetShotsSpreadDegree() const;

	/**
	 *  Sets Turret and Barrel at Idle Position
	 */
	void SetIdle();

	/**
     * Sets Projectile Class that needs to be spawned.On special event Mortar Can have different projectile
     * @param[in] ProjectileToSet Projectile to spawn of Class Projectile
     */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Firing")
	virtual void SetProjectile(const TMap<FName, TSubclassOf<class AActor>>& ProjectileToSet);

	/**
     *   Event Called Just Before Spawning a Projectile actor
     */
	UPROPERTY(BlueprintAssignable)
	FMortarFireEvent OnFireEvent;

protected:

	/**
	 * Called when the game starts
	 */
	virtual void BeginPlay() override;

	/**
	 * Function Called on Every Tick
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * This is a map of Socket Name To Projectile Telling
	 * Which socket has to Fire which projectile
	 * @note We can Use Projectile Class but this will force users to
	 * Use Projectile as base class which We don't want so better to use AActor class
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing")
	TMap<FName, TSubclassOf<class AActor>> ProjectileSpawnSockets;

	/**
	 *  Barrel/Gun Class to Use Moves Vertically
	 */
	class UMortarBarrel* Barrel = nullptr;

	/**
	 *  Turret Class to Use Moves Horizontally
	 */
	class UMortarTurret* Turret = nullptr;

	/**
	 * Sound Played on Each Mortar Firing
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortar|Firing")
	class USoundBase* FireSound;

private:

	/*
	 * Call Back Function For Projectile Destoryed
	 * param[in]  Actor which is destoryed
	 */
	UFUNCTION()
	void ProjectileDestoryed(AActor* DestroyedActor);

	/**
	 * Returns A Valid Subsystem Which has the resourcemanaging Part
	 * This also ensures that the subsystem is the blueprint one and not the cpp one
	 * return   Pointer To SubSystem if valid
	 *           nullptr otherwise
	 */
	class UMortarResourceManager* GetValidSubSystem();


	/**
	 * Move the Barrel at the given Direction
	 * And also Rotates turret
	 * param[in] AimDirection Direction to aim. It is the direction which we estimated 
	 */
	void MoveBarrelTowards(const FVector &AimDirection);

	/**
	 * Sets State of Barrel like Moving,locked etc
	 */
	void SetBarrelState();

	/**
	 * Current Aim Direction of the mortar gun
	 */
	FVector CurrentAimDirection;

	/**
	 *  Time Delay Between each shots
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing", meta = (ClampMin = "0"))
	float ReloadTimeInSeconds = 0.5f;

	/**
	 *  How much shorts can Be Spread in Degrees
	 *  Shots Spread Range between -ShotsSpreadDegree to ShotsSpreadDegree inclusive
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing")
	float ShotsSpreadDegree = 2.f;

	/**
	 * Allow to use Firing Left Only if Unlimited Ammo is off
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing", meta = (EditCondition = "!bUnlimitedAmmo"))
	int32 FiringLeft = 1000;

	/**
	 * Allow Unlimited Firing
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing")
	bool bUnlimitedAmmo = true;

	/**
	 * Stores Current Firing state used by BP
	 */
	//UPROPERTY(BlueprintReadOnly, Category = "Mortar|Firing")
	EMortarFiringState CurrentFiringState;

	/**
	 * This is used to for comparing Aim Direction and Forward Vector of Gun
	 * To check if they are in same direction
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Firing")
	float VectorThresholdComparision = 0.1f;

	/**
	 * Stores Last Firing Time
	 */
	float LastFireTime = 0;

	/**
	 * Original Reload Time In Seconds
	 */
	float OriginalReloadTimeInSeconds = 0.5f;

	/**
	 * Original Firing Left Count
	 */
	int32 OriginalFiringLeft = 1000;

	/**
	 *  Default Aim Locatin of Barrel
	 */
	FVector DefaultAimLocation;

	/** 
	 * Whether Mortar Can Shoot or not
	 */
	uint32 bCanShoot : 1;

};
