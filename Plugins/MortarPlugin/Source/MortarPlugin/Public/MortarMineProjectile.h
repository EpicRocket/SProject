//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MortarProjectile.h"
#include "MortarMineProjectile.generated.h"

/**
 * Mine Bomb Projectile class which blasts after some delay
 */
UCLASS()
class MORTARPLUGIN_API AMortarMineProjectile : public AMortarProjectile
{
	GENERATED_BODY()


public:
	AMortarMineProjectile();


	/**
	 * Event Called After On Hit This is a blueprintImplementableEvent which has to be used in blueprint
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Mortar|Projectile", meta = (DisplayName = "On Hit Before Explosion Event"))
	void OnStartExplosionEvent();

protected:

	/**
	 * After Collision With The Object Blast Timer Will start
	 * @see  https://docs.unrealengine.com/en-US/API/Runtime/Engine/Components/UPrimitiveComponent/OnComponentHit/index.html
	 */
	 void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
			const FHitResult& Hit) override;
	 /**
	  * Delay AFter Which The Projectile Will Blast
	  */
	 UPROPERTY(EditAnywhere, Category = "Mortar|Projectile")
	 float BlastDelay = 2.5f;

private:

	/**
	 *  After How Long The Blast Will Occur
	 */
	void ProjectileBlastTimer();

	/**
	 *  Contains Ignored Actor List
	 */
	TArray<AActor*>IgnoredActor;

	/*
	 * To Ensure we hit only once
	 */
	uint32 bAlreadyHit : 1;
};
