﻿
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/GameStateBase.h"
#include "AbilitySystemInterface.h"

#include "MyGameState.generated.h"

class UMyAbilitySystemComponent;

UCLASS(BlueprintType, Blueprintable)
class SPROJECT_API AMyGameState : public AGameStateBase, public IAbilitySystemInterface
{
    GENERATED_BODY()

    static FName AbilitySystemComponentName;

public:
    AMyGameState();

    //~IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface

private:
    TObjectPtr<UMyAbilitySystemComponent> AbilitySystemComponent;
};