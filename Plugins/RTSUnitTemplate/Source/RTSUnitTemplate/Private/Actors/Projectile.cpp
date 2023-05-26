// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.


#include "Actors/Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Characters/UnitBase.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryAndPhysics
	Mesh->SetMaterial(0, Material);

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Is Projectile Capsule"));
	TriggerCapsule->InitCapsuleSize(30.f, 30.0f);;
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin); 
	

}

void AProjectile::Init(AActor* TargetActor, AActor* ShootingActor)
{
	Target = TargetActor;

	AUnitBase* ShootingUnit = Cast<AUnitBase>(ShootingActor);
	if(ShootingUnit)
	{
		Damage = ShootingUnit->AttackDamage;
		IsFriendly = ShootingUnit->IsFriendly;
		MovementSpeed = ShootingUnit->ProjectileSpeed;
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeTime += DeltaTime;

	if(LifeTime > MaxLifeTime)
	{
		Destroy(true, false);
	}else if(Target)
	{
		AUnitBase* TargetToAttack = Cast<AUnitBase>(Target);
		
		if(TargetToAttack && TargetToAttack->GetUnitState() != UnitData::Dead)
		{
			const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), TargetToAttack->GetActorLocation());
			AddActorLocalOffset(Direction*MovementSpeed);
		}else
		{
			Destroy(true, false);
		}
	}
	
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AUnitBase* UnitToHit = Cast<AUnitBase>(OtherActor);
		if(UnitToHit && UnitToHit->GetUnitState() == UnitData::Dead)
		{
			Destroy(true, false);
		}else if(UnitToHit && UnitToHit->IsFriendly != IsFriendly)
		{
			UnitToHit->SetHealth(UnitToHit->GetHealth()-Damage);
			
			if(UnitToHit->GetUnitState() != UnitData::Run)
			UnitToHit->SetUnitState( UnitData::IsAttacked );
			
			Destroy(true, false);
		}
			
	}
}

