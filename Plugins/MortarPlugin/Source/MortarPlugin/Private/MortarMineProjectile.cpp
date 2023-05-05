///Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarMineProjectile.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "GameFramework/DamageType.h"
#include "NiagaraComponent.h"

AMortarMineProjectile::AMortarMineProjectile()
{
	ProjectileMovement->bShouldBounce = false;
	//ProjectileMovement->Bounciness = 0.01f;
	ProjectileMovement->InitialSpeed = 500;
	ProjectileMovement->MaxSpeed = 500;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Friction = 1;
	CollisionMesh->SetSimulatePhysics(true);
	CollisionMesh->SetEnableGravity(false);
	bAlreadyHit = false;
	InitialLifeSpan = 20.f;
}

//After Hitting A Target
void AMortarMineProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
	const FHitResult& Hit) 
{
	if (OtherActor)
		UE_LOG(LogMortarPlugin, Verbose, TEXT("AMortarMineProjectile Projectile Hit Actor %s"), *OtherActor->GetName());

	//This is to ensure that we run the logic of hit once only
	if (bAlreadyHit)
	{
		return;
	}

	bAlreadyHit = true;

	//Event Called on Start Explosion Event
	//i.e After Hitting Target
	OnStartExplosionEvent();

	IgnoredActor.Empty();

	//Deactivate Launch Blast
	LaunchBlast->Deactivate();
	TrailParticle->Deactivate();
	

	NiagaraLaunchBlast->Deactivate();
	NiagaraTrailParticle->Deactivate();


	AActor* OwnerActor = GetOwner();

	IgnoredActor.Push(OwnerActor);

	//Check for Same Team Damage
	if (bAvoidSameTeamDamage && OwnerActor && OtherActor)
	{
		UMortarTeamComponent* OwnerTeamComponent = Cast<UMortarTeamComponent>(OwnerActor->GetComponentByClass(UMortarTeamComponent::StaticClass()));
		UMortarTeamComponent* OtherActorTeamComponent = Cast<UMortarTeamComponent>(OtherActor->GetComponentByClass(UMortarTeamComponent::StaticClass()));

		//Checking Team ID For Both Actors
		if (OwnerTeamComponent && OtherActorTeamComponent
			&& OwnerTeamComponent->GetTeamInfo().TeamID == OtherActorTeamComponent->GetTeamInfo().TeamID)
		{
			//Ignore Other Actor because Same Team ID
			IgnoredActor.Push(OtherActor);
		}
	}

	//Destroy Projectile after the delay time
	//Timner Handle for destory Projectile

	FTimerHandle ParticleBlastHandler;
	GetWorld()->GetTimerManager().SetTimer(ParticleBlastHandler, this,
		&AMortarMineProjectile::ProjectileBlastTimer,
		BlastDelay, false);

}

void AMortarMineProjectile::ProjectileBlastTimer()
{
	ImpactBlast->Activate();
	NiagaraImpactBlast->Activate();

	ExplosionForce->FireImpulse();
	SetRootComponent(ImpactBlast);


	// Play Impact Sound at the location of Hit
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	//This Method can be used in BluePrint
	ExplosionEvent();

	//Destory Collision Mesh After Hitting 
	if (CollisionMesh)
	{
		CollisionMesh->DestroyComponent();
	}
	


	//This Will cause TakeDamage method of actor if implemented
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		ProjectileDamage,
		GetActorLocation(),
		ExplosionForce->Radius, //for consistency
		UDamageType::StaticClass(),
		IgnoredActor
	);

	//Destroy Projectile after the delay time
	//Timner Handle for destory Projectile
	FTimerHandle ParticleDestroyer;
	GetWorld()->GetTimerManager().SetTimer(ParticleDestroyer, this,
		&AMortarMineProjectile::ProjectileDestroyerTimer,
		DestroyDelay, false);
}