//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarHealthComponent.h"
#include "MortarPlugin.h"


// Sets default values for this component's properties
UMortarHealthComponent::UMortarHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame. 
	 //You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bIsCriticalHealthEventCalled = false;
	bIsDeadEventCalled = false;


}

void UMortarHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	float Health = MaxHealth;
	CriticalHealthThreshold = Health * CriticalHealthPercentage / 100.f;
	

	AActor* Owner = GetOwner();

	if (Owner)
	{
		//eRegister For Damage Event
		Owner->OnTakeAnyDamage.AddDynamic(this, &UMortarHealthComponent::TakeDamage);
	}
	else
	{
		UE_LOG(LogMortarPlugin, Warning, TEXT(" HealthComponent - There is No Owner For This Component"));
	}

}

void UMortarHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	//We want health in integers
	int32 DamagePoints = FPlatformMath::RoundToInt(Damage);
	int32 ActualDamage = FMath::Clamp<int32>(DamagePoints, 0, MaxHealth);

	CurrentHealth -= ActualDamage;
	CheckIsCriticalHealth();


}

void UMortarHealthComponent::ResetHealth()
{
	CurrentHealth = DefaultHealth;
	CheckIsCriticalHealth();
}

bool UMortarHealthComponent::IsDead() const
{
	if (CurrentHealth > 0)
	{
		return false;
	}

	return true;
}

#if WITH_EDITOR
void UMortarHealthComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	DefaultHealth = FMath::Clamp(DefaultHealth, 0, MaxHealth);
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UMortarHealthComponent::CheckIsCriticalHealth()
{
	//Todo Make it More Elegant
	if (CurrentHealth <= 0 && bIsDeadEventCalled == false)
	{
		bIsDeadEventCalled = true;
		ActorDeadEvent.Broadcast();
	}

	else
	{
		if (CurrentHealth <= CriticalHealthThreshold && bIsCriticalHealthEventCalled == false)
		{
			bIsCriticalHealthEventCalled = true;
			bIsDeadEventCalled = false;
	        CriticalHealthEvent.Broadcast();
		}
		else if (CurrentHealth >= CriticalHealthThreshold && bIsCriticalHealthEventCalled == true)
		{
			bIsCriticalHealthEventCalled = false;
			bIsDeadEventCalled = false;
			CrticalHealthRecoverEvent.Broadcast();
		}
	}
}

//setters
void UMortarHealthComponent::SetCurrentHealth(const int32& Health)
{
	CurrentHealth = FMath::Clamp(Health, 0, MaxHealth);
	CheckIsCriticalHealth();
}

void UMortarHealthComponent::SetMaxHealth(const int32& Health)
{
	MaxHealth = Health;
}

void UMortarHealthComponent::SetDefaultHealth(const int32& Health)
{
	DefaultHealth = FMath::Clamp(Health, 0, MaxHealth);
}

//Geters
int32 UMortarHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

int32 UMortarHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

int32 UMortarHealthComponent::GetDefaultHealth() const
{
	return DefaultHealth;
}

float UMortarHealthComponent::GetHealthPercentage() const
{
	return (float)(CurrentHealth) / (float)(MaxHealth);
}

float UMortarHealthComponent::GetCriticalHealthThresholdPercentage() const
{
	return (float)(CriticalHealthThreshold) / (float)(MaxHealth);
}
