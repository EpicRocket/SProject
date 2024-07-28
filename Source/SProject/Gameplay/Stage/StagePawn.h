
#pragma once

#include "GameFramework/Pawn.h"

#include "StagePawn.generated.h"

class AController;
struct FGameplayMessageListenerHandle;

UCLASS(Abstract, BlueprintType, Blueprintable, hideCategories=(Navigation), MinimalAPI)
class AStagePawn : public APawn
{
    GENERATED_BODY()

public:
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

private:
    UPROPERTY(Transient)
    TArray<FGameplayMessageListenerHandle> GameplayMessageListenerHandles;
};
