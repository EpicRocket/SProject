

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
//#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Team/GameTeamAgentInterface.h"
#include "Unit.generated.h"

UCLASS()
class SPROJECT_API AUnit : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AUnit();

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	//~ End IAbilitySystemInterface

	//~ Begin IAbilitySystemInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const;
	//~ End IAbilitySystemInterface
};
