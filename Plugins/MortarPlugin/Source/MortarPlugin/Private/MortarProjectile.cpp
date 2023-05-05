//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MortarTeamComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/DamageType.h"
#include "NiagaraComponent.h"


// Sets default values
AMortarProjectile::AMortarProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(true);
	SetRootComponent(CollisionMesh);

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Launch Blast"));
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	//for Trail
	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("Trail Particle"));
	TrailParticle->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	//for impact
	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact Blast"));
	ImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactBlast->bAutoActivate = false;

	//Niagara Particle Systems

	NiagaraLaunchBlast = CreateDefaultSubobject<UNiagaraComponent>(FName("Niagara Launch Blast"));
	NiagaraLaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//for Trail
	NiagaraTrailParticle = CreateDefaultSubobject<UNiagaraComponent>(FName("Niagara Trail Particle"));
	NiagaraTrailParticle->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	//for impact
	NiagaraImpactBlast = CreateDefaultSubobject<UNiagaraComponent>(FName("Niagara Impact Blast"));
	NiagaraImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	NiagaraImpactBlast->bAutoActivate = false;

	//Creates Impact Force
	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Radial Force"));
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement Component"));
	ProjectileMovement->bAutoActivate = true;

	ProjectileMovement->InitialSpeed = 500;
	ProjectileMovement->MaxSpeed = 500;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	InitialLifeSpan = 10.f;
	

}

// Called when the game starts or when spawned
void AMortarProjectile::BeginPlay()
{
	Super::BeginPlay();

	//Add On Hit Event
	if (CollisionMesh)
	{
		CollisionMesh->OnComponentHit.AddDynamic(this, &AMortarProjectile::OnHit);
	}
	
}

//After Hitting A Target
void AMortarProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
							  UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
		                      const FHitResult& Hit)
{

	if(OtherActor)
		UE_LOG(LogMortarPlugin, Verbose, TEXT("Projectile Hit Actor %s"), *OtherActor->GetName());

	//Deactivate Launch Blast
	LaunchBlast->Deactivate();
	TrailParticle->Deactivate();
	ImpactBlast->Activate();

	NiagaraLaunchBlast->Deactivate();
	NiagaraTrailParticle->Deactivate();
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
	CollisionMesh->DestroyComponent();


	AActor* OwnerActor = GetOwner();
	
	TArray<AActor*>IgnoredActor;
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
										   &AMortarProjectile::ProjectileDestroyerTimer,
										   DestroyDelay,false);
}

//Destory ACtor
void AMortarProjectile::ProjectileDestroyerTimer()
{
	Destroy();
}

void AMortarProjectile::SetProjectileDamage(const float& InProjectileDamage)
{
	ProjectileDamage = InProjectileDamage;
}

void AMortarProjectile::SetDestroyDelay(const float& Delay)
{
	DestroyDelay = Delay;
}

void AMortarProjectile::SetAvoidSameTeamDamage(const bool& ShouldAvoidSameDamage)
{
	bAvoidSameTeamDamage = ShouldAvoidSameDamage;
}

float AMortarProjectile::GetProjectileDamage() const
{
	return ProjectileDamage;
}

float AMortarProjectile::GetDestoryDelay() const
{
	return DestroyDelay;
}

bool AMortarProjectile::ShouldAvoidSameTeamDamage() const
{
	return bAvoidSameTeamDamage;
}

