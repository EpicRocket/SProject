
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemInterface.h"

#include "MyGameState.generated.h"

class UGAbilitySystemComponent;

UCLASS(BlueprintType, Blueprintable)
class MY_API AMyGameState : public AGameState, public IAbilitySystemInterface
{
    GENERATED_BODY()

    static FName AbilitySystemComponentName;

public:
    AMyGameState();

    //~IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface

private:
    TObjectPtr<UGAbilitySystemComponent> AbilitySystemComponent;
};
