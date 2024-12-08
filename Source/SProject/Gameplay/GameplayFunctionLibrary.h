
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayFunctionLibrary.generated.h"

class AUnitCharacter;

UCLASS()
class MY_API UGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
   /* UFUNCTION(BlueprintCallable, Category = "Gameplay")
    static AUnitCharacter* SpawnUnit(TSubclassOf<AUnitCharacter> UnitClass, const FTransform& Transform);

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    static void RemoveUnit(AUnitCharacter* Unit);*/
};
