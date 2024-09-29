
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayWorldSubsystem.generated.h"

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
};
