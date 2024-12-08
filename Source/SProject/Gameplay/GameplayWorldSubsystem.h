
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayWorldSubsystem.generated.h"

class AUnitCharacter;
class AAIController;

UCLASS()
class MY_API UGameplayWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

};

UCLASS()
class MY_API UGameplayHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "게임플레이|Helper", meta = (WorldContext = "WorldContextObject"))
    static AUnitCharacter* SpawnUnit(UObject* WorldContextObject, FVector Location, FRotator Rotation, TSubclassOf<AUnitCharacter> UnitClass, TSubclassOf<AAIController> AIController);
};
