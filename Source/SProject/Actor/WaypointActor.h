
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "WaypointActor.generated.h"

class USuperAbilitySystemComponent;

UCLASS(BlueprintType, hideCategories = (Input, Rendering))
class AWaypointActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()

public:
	// IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~IAbilitySystemInterface interface

private:

};
