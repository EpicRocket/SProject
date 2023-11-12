

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
// #include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Team/GameTeamAgentInterface.h"
#include "Unit.generated.h"

class USuperAbilitySystemComponent;

UCLASS()
class SPROJECT_API AUnit : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AUnit();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	//~ IAbilitySystemInterface

	// IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const;
	//~ IGameplayTagAssetInterface

	virtual void UnitInitailize();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGuid UnitId;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	USuperAbilitySystemComponent* AbilitySystemComponent;
};

bool IsValidUnit(AUnit* Unit);
