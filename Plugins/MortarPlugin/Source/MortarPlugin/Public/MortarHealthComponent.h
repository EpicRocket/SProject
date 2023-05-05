//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MortarHealthComponent.generated.h"

/*
 * Event Generated Whenever Life of mortar Drops beyond a critical limit
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarCriticalHealthEvent);

/*
 * Event Generated Whenever Life of mortar Goes beyond critical limit
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarCriticalHealthRecoverEvent);

/*
 * Event Generated Whenever Actor Dies
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMortarDeadEvent);

/*
 *  This is a Generic Health ACtor Component which can be added to any character
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MORTARPLUGIN_API UMortarHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMortarHealthComponent();

	/**
	 *   Returns The Current Health of the ACtor
	 *   @return Current Health which is clamped to max Health
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;

	/**
	 *   Checks if the health is critical and calls a blueprint event
	 *   if it gets low or stops critical Health event when health restores
	 */
	void CheckIsCriticalHealth();

	/**
	 *   Sets The Current Health of the ACtor
	 *   @param[in] Health Current Health which is clamped to max Health
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(const int32& Health);

	/**
	 *   Resets The Actor Health to the Default Health
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	/**
	 *   Sets The Maximum Health an actor can achieve
	 *   @param[in] Health Maximum Health
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(const int32& Health);

	/**
	 *   Sets The Default Health an actor This will also be clamped to MaxHealth
	 *   @param[in] Health Default Health of an actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetDefaultHealth(const int32& Health);

	/**
	 *    Blueprint Event which is called when health drops beyond a critical threshold
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarCriticalHealthEvent CriticalHealthEvent;

	/**
	 *    Blueprint Event which is called when health restores and gets above a critical threshold
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarCriticalHealthRecoverEvent CrticalHealthRecoverEvent;

	/**
	 *    Blueprint Event which is called when Actor Dies
	 */
	UPROPERTY(BlueprintAssignable)
	FMortarDeadEvent ActorDeadEvent;

	/**
	 *   Getter function for Max Health
	 *   @return[in] Max Health of an actor
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetMaxHealth() const;

	/**
	 *   Getter function for Default Health
	 *   @return Default Health of an actor
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetDefaultHealth() const;

	/**
	 *   Returns Health Left Percentage.
	 *   @return Health Percentage in float
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;

	/**
	 *   Returns Critical Health Percentage.
	 *   @return CriticalHealth Percentage in float
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCriticalHealthThresholdPercentage() const;

	/**
	 *   Checks if an actor is dead by checking the current Health
	 *   @return Returns True if Current Health less than and equal to 0, false otherwise
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

protected:

	/**
	 * Called when the game starts or when spawned
	 */
	virtual void BeginPlay() override;

	/**
	 * Function to afflict damage This will be called whener the owning actor
	 * Receives damage
	 * @param[in] DamagedActor  Actor which has been damaged
	 * @param[in] Damage  Damage Amount
	 * @param[in] DamageType  Type of Damage More Info on Epic Link @see https://docs.unrealengine.com/en-US/API/Runtime/Engine/GameFramework/UDamageType/index.html
	 * @param[in] InstigatedBy  Controller of the Instigator
	 * @param[in] DamageCauser Damage Causer Here In our example it will be Projectile Actor
	 */
	UFUNCTION()
	virtual void TakeDamage(AActor* DamagedActor, float Damage,
							const class UDamageType* DamageType,
						    class AController* InstigatedBy,
						    AActor* DamageCauser);


#if WITH_EDITOR

	/**
	 * Function Called post changing the property of widgets in editor
	 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	/**
	 * Default Health of an Actor
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Health")
	int32 DefaultHealth = 100;

	/**
	 * Current Health of an Actor
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Health")
	int32 CurrentHealth = 100;

	/**
	 * Maximum Health of an Actor
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Health")
	int32 MaxHealth = 100;

	/**
	 * This is the percentage beyond which the critical Health event will be called
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Health", meta = (ClampMin = "0",ClampMax="100"))
	float CriticalHealthPercentage = 25;

	/**
	 * Critical Health Threshold Limit beyond which an event will be generated
	 * It will be CriticalHealthPercentage * MaxHealth
	 */
	float CriticalHealthThreshold = 20;

	/**
	 * Boolean Flag to check whether the event for critical Health is called
	 */
	uint32 bIsCriticalHealthEventCalled : 1;

	/**
	 * Boolean Flag to check whether the event for Death is called
	 */
	uint32 bIsDeadEventCalled : 1;

};
