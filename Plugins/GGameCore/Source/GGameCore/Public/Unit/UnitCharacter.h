
#pragma once

#include "GameFramework/Character.h"
#include "Team/Interface/IGTeamAgent.h"
#include "AbilitySystemInterface.h"

#include "UnitCharacter.generated.h"

class UGBaseAttributeSet;
class UObject;

UCLASS(Abstract, Config = Game, BlueprintType, Blueprintable)
class GGAMECORE_API AUnitCharacter : public ACharacter, public IGTeamAgent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUnitCharacter();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~IGTeamAgent

	// IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

	UFUNCTION(BlueprintCallable)
	virtual void Remove();

	UFUNCTION(BlueprintCallable)
	virtual void Kill();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Unit")
	FGuid UnitId;

	UPROPERTY(EditInstanceOnly, Category = "Team")
	uint8 TeamID;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Unit")
	TSubclassOf<class UAnimInstance> DefaultAnimClassLayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	TObjectPtr<class UGAbilitySystemComponent> AbilitySystemComponent;
	
};
