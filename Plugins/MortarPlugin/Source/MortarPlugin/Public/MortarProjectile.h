//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MortarProjectile.generated.h"

/**
 * Projectile Base class which includes all the logic of projectile launching damaging etc
 */
UCLASS()
class MORTARPLUGIN_API AMortarProjectile : public AActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AMortarProjectile();

	/**
	 * Event Called Before Doing Blast This is a  blueprintImplementableEvent which has to be used in blueprint
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortar|Projectile", meta = (DisplayName = "After Hit Explosion Event"))
	void ExplosionEvent();

	/**
	 * Sets Projectile Done by each projectile Hit
	 * @param[in] ProjectileDamage Projectile Damage Amount
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Projectile")
	void SetProjectileDamage(const float& ProjectileDamage);

	/**
	 * Sets Time after which each Projectile will get destroyed after hitting the target
	 * Avoid Keeping the value too big for performance issue
	 * @param[in] DestroyDelay Projectile Destroy Delay
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Projectile")
	void SetDestroyDelay(const float& DestroyDelay);

	/**
	 * Sets whether same team damage is allowed or not
	 * @param[in] ShouldAvoidSameDamage Boolean Value true to avoid same team damage ,false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Mortar|Projectile")
	void SetAvoidSameTeamDamage(const bool& ShouldAvoidSameDamage);

	/**
	 *  Gets Damage Done by Each Projectile Hit on the Character
	 *  return  Float Variable Containing Projectile Damage 
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Projectile")
	float GetProjectileDamage() const;

	/**
	 *  Gets Destory Delay for Each Projectile After it hits a surface
	 *  return  Float Variable Containing Projectile Delay
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Projectile")
	float GetDestoryDelay() const;


	/**
	 *   Returns True if Same Team Damage is off, false otherwise
	 *   @return Returns True if same team damage is not allowed
	 */
	UFUNCTION(BlueprintPure, Category = "Mortar|Projectile")
	bool ShouldAvoidSameTeamDamage() const;

protected:

	/**
	 * Called when the game starts or when spawned
	 */
	virtual void BeginPlay() override;

	
	/**
	 * On Every Collision of Projectile This Function will be called.This is Called on ComponentHit 
	 * Function of Collision Mesh of Projectile Mesh. 
	 * @see  https://docs.unrealengine.com/en-US/API/Runtime/Engine/Components/UPrimitiveComponent/OnComponentHit/index.html
	 */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	 		           UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
			           const FHitResult& Hit);

	/**
	 * Projectile Movement For Movement of Projectile
	 * @see https://docs.unrealengine.com/en-US/API/Runtime/Engine/GameFramework/UProjectileMovementComponent/index.html
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	class UProjectileMovementComponent* ProjectileMovement = nullptr;

	/**
	 * Mesh of projectile Which will enable collision event
	 */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Mortar|Projectile")
	UStaticMeshComponent* CollisionMesh = nullptr;

	/**
	 * Particle System That will be emitted on Launch from Mortar Gun
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	UParticleSystemComponent* LaunchBlast = nullptr;

	/**
	 * Particle System That will be used on the Projectile Mesh like Smoke Trail etc
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	UParticleSystemComponent* TrailParticle = nullptr;

	/**
	 * Particle System That will be emitted on impact
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	UParticleSystemComponent* ImpactBlast = nullptr;

	/**
	 * Particle System That will be emitted on Launch from Mortar Gun
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	class UNiagaraComponent* NiagaraLaunchBlast = nullptr;

	/**
	 * Particle System That will be used on the Projectile Mesh like Smoke Trail etc
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	class UNiagaraComponent* NiagaraTrailParticle = nullptr;

	/**
	 * Particle System That will be emitted on impact
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	class UNiagaraComponent* NiagaraImpactBlast = nullptr;

	/**
	 * This is impulse Force x change in time see wikipedia
	 */
	UPROPERTY(VisibleAnywhere, Category = "Mortar|Projectile")
	class URadialForceComponent* ExplosionForce = nullptr;

	/**
	 * Sound Played on Particle Hit
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mortar|Projectile")
	class USoundBase* ImpactSound;

	/**
	 * Whether To Avoid Same Team Damage
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Projectile")
	bool bAvoidSameTeamDamage = false;

	/**
	 * Time after which projectiles will be deleted. Avoid keep it too large
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Projectile")
	float DestroyDelay = 2.f;

	/**
	 * Damage To Be Done By each Projectile on Successfull Hit
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Projectile")
	float ProjectileDamage = 10.f;

	/**
	 * Function To Delete Projectiles Actor
	 */
	virtual void ProjectileDestroyerTimer();

};
