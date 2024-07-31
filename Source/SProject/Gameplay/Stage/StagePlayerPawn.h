
#pragma once

#include "GameFramework/Pawn.h"

#include "StagePlayerPawn.generated.h"

class AController;
struct FGameplayMessageListenerHandle;

UENUM(BlueprintType, meta = (Bitflags))
enum class EStagePlayerFlag : uint8
{
    None                UMETA(DisplayName = "None"),
    Build               UMETA(DisplayName = "건설"),
};

UCLASS(Abstract, BlueprintType, Blueprintable, hideCategories=(Navigation))
class MY_API AStagePlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

private:
    void OnUpdateState();

private:
    UPROPERTY(Transient)
    TArray<FGameplayMessageListenerHandle> GameplayMessageListenerHandles;

    UPROPERTY(Transient, meta = (Bitmask, BitmaskEnum = EStagePlayerFlag))
    int32 PlayerFlags = 0;
};
