//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarAimingComponent.h"
#include "MortarBarrel.h"
#include "MortarTurret.h"
#include "MortarProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Kismet/KismetMathLibrary.h"
#include "MortarResourceManager.h"
#include "Kismet/GameplayStatics.h"
#include "MortarPlugin.h"


// Sets default values for this component's properties
UMortarAimingComponent::UMortarAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentFiringState = EMortarFiringState::Reloading;
	bCanShoot = false;

}

void UMortarAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	//Store Last Firing Time That will be used for calculating Fire Rate
	LastFireTime = -100;// GetWorld()->GetTimeSeconds();


	OriginalReloadTimeInSeconds = ReloadTimeInSeconds;
	OriginalFiringLeft = FiringLeft;

	DefaultAimLocation = FVector::ZeroVector;

}

void UMortarAimingComponent::TickComponent(float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	//Set Whether Barrel is locked or moving etc
	SetBarrelState();
}

//Set Barrel State at every tick
void UMortarAimingComponent::SetBarrelState()
{
	if (!bUnlimitedAmmo && FiringLeft <= 0)
	{
		CurrentFiringState = EMortarFiringState::OutOfAmmo;
	}

	else if ((GetWorld()->GetTimeSeconds() - LastFireTime) < ReloadTimeInSeconds)
	{
		CurrentFiringState = EMortarFiringState::Reloading;
	}

	//If Barrel is moving Then It means it is still aiming
	else if (IsBarrelMoving())
	{
		CurrentFiringState = EMortarFiringState::Aiming;
	}

	//Only In this state we will allow Firing
	else
	{
		CurrentFiringState = EMortarFiringState::Locked;
	}
}

//If Current Aim Direction wanted is same as Barrel current
//forward vector then this means barrel is stationary and is ready for firing
bool UMortarAimingComponent::IsBarrelMoving()
{
	
	if (Barrel == nullptr)
	{
		return false;
	}

	//Get Rotation To Turret
	FRotator TurretRotation = UKismetMathLibrary::FindLookAtRotation(Turret->GetComponentLocation(), CurrentAimDirection);
	//Get Rotation To Barrel
	FRotator BarrelRotation = UKismetMathLibrary::FindLookAtRotation(Barrel->GetComponentLocation(), CurrentAimDirection);

	

	//Check IF Barrel and Turret Both are poiting to the desired Location
	if (FMath::IsNearlyEqual(FMath::Abs(TurretRotation.Yaw), 
		FMath::Abs(Turret->GetComponentRotation().Yaw), VectorThresholdComparision) &&
		FMath::IsNearlyEqual(FMath::Abs(BarrelRotation.Pitch), 
		FMath::Abs(Barrel->GetComponentRotation().Pitch), VectorThresholdComparision)) // Match Both Vector 
	{
		return false;
	}

	return true;
}

//To Be Called From BLuePrint
void UMortarAimingComponent::InitializeAiming(UMortarTurret* TurretToSet, UMortarBarrel* BarrelToSet)
{
	Barrel = BarrelToSet;
	Turret = TurretToSet;


	if (!ensure(Barrel) || !ensure(Turret))
	{
		return;
	}

	//Check If Socket Exists
	for (auto It = ProjectileSpawnSockets.CreateConstIterator(); It; ++It)
	{
		FName SocketName = It.Key();
		if (It.Value() && Barrel->DoesSocketExist(SocketName))
		{
			//Sets The Default Socket Location
			DefaultAimLocation = Barrel->GetSocketLocation(SocketName) + Barrel->GetForwardVector() * 100;
			bCanShoot = true;
			return;
		}
	}

	UE_LOG(LogMortarPlugin, Warning, TEXT("Projectile Socket Missing From Gun.Please Add from Socket Manager"));
	bCanShoot = false;
}


//Aim At The Desired Location
void UMortarAimingComponent::AimAt(const FVector& HitLocation)
{
	if (!ensure(Barrel))
	{
		return;
	}

	//If Hit Location is found then Move Barrel
	if (bCanShoot)
	{
		CurrentAimDirection = HitLocation;
		MoveBarrelTowards(CurrentAimDirection);
	}
	//Do nothing if no solution found
}

//Sets Gun at Idle Position
void UMortarAimingComponent::SetIdle()
{
	MoveBarrelTowards(DefaultAimLocation);
}


void UMortarAimingComponent::ProjectileDestoryed(AActor* Act)
{
	if (!Act)
		return;


		UMortarResourceManager* SubSystem = GetValidSubSystem();
		if (SubSystem)
		{
			SubSystem->RemoveFromPool(Act, Act->GetClass());
		}

}

//This Will Rotate Barrel and Elevate or lower the turret
void UMortarAimingComponent::MoveBarrelTowards(const FVector& AimDirection)
{
	if (!ensure(Barrel) || !ensure(Turret))
	{
		return;
	}

	//Move Barrel and Turret To the desired Rotation
	Turret->Azimuth(AimDirection);
	Barrel->Elevate(AimDirection);

}

//Returnbs THe desired Subsystem. Return Null if the subsystem is not
//the blueprint one since all values are set in blueprint one
UMortarResourceManager* UMortarAimingComponent::GetValidSubSystem()
{
	UMortarResourceManager* Subsystem = nullptr;
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMortarResourceManager>();
	}

	return Subsystem;

}
//Ready To Engage
void UMortarAimingComponent::Fire()
{
	//Allow Firing In locked State
	if (CurrentFiringState == EMortarFiringState::Locked)
	{

		if (!ensure(Barrel) || ProjectileSpawnSockets.Num() == 0 || !bCanShoot)
		{
			return;
		}

		AActor* Owner = GetOwner();
		if (Owner)
			UE_LOG(LogMortarPlugin, Verbose, TEXT("%s Firing At Target"), *Owner->GetName());

		//Broad Cast Fire Event 
	    //This can be handled in blueprint and we can things like
		//Recoiling
		OnFireEvent.Broadcast();

		// Play Fire Sound at the location of Barrel
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Barrel->GetComponentLocation());
		}

		for (auto It = ProjectileSpawnSockets.CreateConstIterator(); It; ++It)
		{
			FName SocketName = It.Key();
			//Check If Socket Exists
			if (!Barrel->DoesSocketExist(SocketName))
			{
				continue;
			}


			//Spawn a projectile at the location of socket
			//We will set some basic parameters that can be used in projectile 
			//More Parameters can be added
			//We can Use Projectile Class but this will force users to 
			//Use Projectile as base class which We don't want so better to use
			//AActor class
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = Owner;
			ActorSpawnParams.Instigator = Owner->GetInstigator();
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


			FRotator rot = Barrel->GetSocketRotation(SocketName);

			//ADd Randomness
			rot.Add(FMath::RandRange(-ShotsSpreadDegree, ShotsSpreadDegree),
				FMath::RandRange(-ShotsSpreadDegree, ShotsSpreadDegree),
				FMath::RandRange(-ShotsSpreadDegree, ShotsSpreadDegree));

			//const AActor* SpawningProjectileActor = It.Value();
			AActor* ProjectileObj = GetWorld()->SpawnActor<AActor>(
				It.Value(),
				Barrel->GetSocketLocation(SocketName),
				rot, ActorSpawnParams
				);

			//Adding Signal On Projectile Destory
			ProjectileObj->OnDestroyed.AddDynamic(this, &UMortarAimingComponent::ProjectileDestoryed);

			UMortarResourceManager* SubSystem = GetValidSubSystem();

			//Adding the projectile To the Pool .This will delete the actor if pool limit exceeds
			if (SubSystem)
			{
				SubSystem->AddToPool(ProjectileObj, ProjectileObj->GetClass());
			}


			if (!bUnlimitedAmmo)
			{
				FiringLeft--;
			}

			if (FiringLeft <= 0)
				break;
		}
		LastFireTime = GetWorld()->GetTimeSeconds();
		//Only if flag is off then reduce the count

	}

}

void UMortarAimingComponent::ResetFiring()
{
	//Original Attributes
	ReloadTimeInSeconds = OriginalReloadTimeInSeconds;
	FiringLeft = OriginalFiringLeft;

}


void UMortarAimingComponent::SetReloadTimeInSeconds(const float& ReloadTime)
{
	ReloadTimeInSeconds = ReloadTime;
}

void UMortarAimingComponent::SetFiringLeft(const int32& InFiringLeft)
{
	FiringLeft = InFiringLeft;
}

void UMortarAimingComponent::SetProjectile(const TMap<FName, TSubclassOf<class AActor>>& ProjectileToSet)
{
	ProjectileSpawnSockets = ProjectileToSet;
}

void UMortarAimingComponent::SetShotsSpreadDegree(const float& ShotsSpread)
{
	ShotsSpreadDegree = ShotsSpread;
}

//Getters
int32 UMortarAimingComponent::GetFiringCount() const
{
	return FiringLeft;
}

int32 UMortarAimingComponent::GetReloadTimeInSeconds() const
{
	return ReloadTimeInSeconds;
}

EMortarFiringState UMortarAimingComponent::GetCurrentState() const
{
	return CurrentFiringState;
}

bool UMortarAimingComponent::HasUnlimitedAmmo() const
{
	return bUnlimitedAmmo;
}

float UMortarAimingComponent::GetShotsSpreadDegree() const
{
	return ShotsSpreadDegree;
}

