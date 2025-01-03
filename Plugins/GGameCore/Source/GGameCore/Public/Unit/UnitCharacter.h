﻿
#pragma once

#include "GameFramework/Character.h"
#include "Team/Interface/IGTeamAgent.h"
#include "AbilitySystemInterface.h"

#include "UnitCharacter.generated.h"

class UGBaseAttributeSet;
class UObject;

UCLASS()
class GGAMECORE_API AUnitCharacter : public ACharacter, public IGTeamAgent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUnitCharacter();

	virtual void BeginPlay() override;

	// IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Unit")
	FGuid UnitId;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Unit")
	TSubclassOf<class UAnimInstance> DefaultAnimClassLayer;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const class UGBaseAttributeSet> BaseAttributeSet;
};
