//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "Mortar.h"
#include "MortarTurret.h"
#include "MortarBarrel.h"
#include "Components/WidgetComponent.h"
#include "MortarHealthComponent.h"
#include "MortarAimingComponent.h"
#include "MortarTeamComponent.h"
#include "TimerManager.h"
#include "MortarPlugin.h"
#include "Components/PawnNoiseEmitterComponent.h"


// Sets default values
AMortar::AMortar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	//Set Mortar Base as root
	MortarBase = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mortar Base"));
	SetRootComponent(MortarBase);

	//ADd Mortar Turret
	MortarTurret = CreateDefaultSubobject<UMortarTurret>(FName("Mortar Turret"));
	MortarTurret->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Lastly Add Gun from which we can fire projectiles
	MortarGun = CreateDefaultSubobject<UMortarBarrel>(FName("Mortar Gun"));
	MortarGun->AttachToComponent(MortarTurret, FAttachmentTransformRules::KeepRelativeTransform);

	//Health Bar Widget
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(FName("Health Bar"));
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize(FVector2D(100, 20));
	HealthBar->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBar->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Rest Components
	MortarHealthComponent = CreateDefaultSubobject<UMortarHealthComponent>(FName("Health Component"));
	MortarAimingComponent = CreateDefaultSubobject<UMortarAimingComponent>(FName("Mortar Aiming Component"));
	MortarTeamComponent = CreateDefaultSubobject<UMortarTeamComponent>(FName("Mortar Team Component"));
	MortarNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(FName("Mortar Noise Component"));

}

// Called when the game starts or when spawned
void AMortar::BeginPlay()
{
	Super::BeginPlay();

	//Set Reference
	if (MortarAimingComponent)
	{
		MortarAimingComponent->InitializeAiming(MortarTurret, MortarGun);
	}

	//No use of Respawn Count if bUnlimited Respawn is on so setting to 1
	if (bUnlimitedRespawn)
	{
		RespawnCount = 1;
	}

	ensure(MortarTeamComponent);

	//Register for Team Change Event Done
	if (MortarTeamComponent)
	{
		MortarTeamComponent->OnTeamChange.AddUniqueDynamic(this, &AMortar::OnTeamChangeDelegate);
	}
	

}

//Damage Event
float AMortar::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		                  AController* EventInstigator, AActor* DamageCauser)
{
	if (!MortarHealthComponent)
	{
		return 0.f;
	}

	UE_LOG(LogMortarPlugin, Verbose, TEXT("Damage %f Applied To Actor %s"),DamageAmount,*GetName());
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	int32 CurrentHealth = MortarHealthComponent->GetHealth();
	if (CurrentHealth <= 0)
	{
		if (DestroyInProgress == 0 && RespawnInProgress == 0)
		{
			//Event Called in Blueprint on actor Death
			ActorDeathEvent();
			//Respawn Till respawn Count is not zero
			if (RespawnCount > 0)
			{

				//Pause Controller for Both Player AI and Player
				OnMortarPause.Broadcast();

				FTimerHandle RespawnTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this,
					&AMortar::StartMortarRespawn,
					DestroyMortarDelay, false);
				RespawnInProgress = 1;
			}
			//As soon as respawn Count Reaches 0 then destroy the actor
			else
			{
				//Listening Parties will respond accordingly
				OnMortarDeath.Broadcast();
				//DetachFromControllerPendingDestroy();

				FTimerHandle DestroyTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this,
					&AMortar::MortarDestroy,
					DestroyMortarDelay, false);
				DestroyInProgress = 1;
			}
		}
	}

	return DamageAmount;
}
//Can be made more efficent
//By maintaining total projectile count if it is greater than 
//fixed amount then delete them
void AMortar::MortarDestroy()
{
	UE_LOG(LogMortarPlugin, Verbose, TEXT("Actor %s Destroyed From World"),*GetName());
	Destroy();
}


void AMortar::StartMortarRespawn()
{
	
	//Event Called in Blueprint on actor respawning
	//Hre we want to Play Effects or sound
	OnActorStartRespawnEvent();

	FTimerHandle AfterRespawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(AfterRespawnTimerHandle, this,
		&AMortar::AfterMortarRespawn,
		RespawnShootingDelay, false);

}

void AMortar::AfterMortarRespawn()
{

	//Reset Component To Default Values after Death 
	if (MortarTeamComponent)
	{
		MortarTeamComponent->ResetTeamInfo();
	}

	if (MortarHealthComponent)
	{
		MortarHealthComponent->ResetHealth();
	}

	if (MortarAimingComponent)
	{
		MortarAimingComponent->ResetFiring();
	}

	//If we allow unlimited respawning there is no need to decrease count
	if (!bUnlimitedRespawn)
	{
		RespawnCount--;
	}

	RespawnInProgress = 0;

	//Broadcase to controllers both player and AI so that they can resume working
	OnMortarResume.Broadcast();

	MortarNoiseEmitter->MakeNoise(this, 1, GetActorLocation());

	//Blueprint Event To tell respawned done
	OnActorEndRespawnEvent();

	UE_LOG(LogMortarPlugin, Verbose, TEXT("Actor %s Respawned In World"), *GetName());

}

//Event Called on Team Change Event
void AMortar::OnTeamChangeDelegate()
{
	//Blueprint Event
	ActorTeamChangeEvent();
}

//We will be ray tracing through the Gun with the help of Socket setup in gun instead of middle 
//Makes More sense and is better in detecting other mortar Actors
bool AMortar::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
		                    int32& NumberOfLoSChecksPerformed, float& OutSightStrength,
		                    const AActor* IgnoreActor, const bool* bWasVisible,
	                        int32* UserData) const
{


	static const FName NAME_AILineOfSight = FName(TEXT("MortarLineOfSight"));
	FHitResult HitResult;


	TArray<FName> sockets = MortarGun->GetAllSocketNames();
	FVector TargetLocation;
	for (int32 i = 0; i < sockets.Num(); i++)
	{
		TargetLocation = MortarGun->GetSocketLocation(sockets[i]);
		break;
	}

	//If we can't find any Socket for the Mortar Gun
	//Then We will pick the actor location.
	if (TargetLocation.IsZero())
	{
		TargetLocation = GetActorLocation();
	}

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, TargetLocation,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));
	NumberOfLoSChecksPerformed++;


	if (bHit == false || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = TargetLocation;
		OutSightStrength = 1.f;
		return true;
	}
	OutSightStrength = 0;
	return false;
}

//Setter Functions
void AMortar::SetDestroyMortarDelay(const float& Delay)
{
	DestroyMortarDelay = Delay;
}

void AMortar::SetUnlimitedRespawn(const bool& UnlimitedRespawning)
{
	bUnlimitedRespawn = UnlimitedRespawning;
}

void AMortar::SetRespawnCount(const uint8& Count)
{
	RespawnCount = Count;
}

void AMortar::SetResourceManager(UMortarResourceManager* resourceManager)
{
	ResourceManager = resourceManager;
}

//Getter Functions
float AMortar::GetDestroyMortarDelay() const
{
	return DestroyMortarDelay;
}

bool AMortar::IsUnlimitedRespawnable() const
{
	return bUnlimitedRespawn;
}

uint8 AMortar::GetRespawnCount() const
{
	return RespawnCount;
}
