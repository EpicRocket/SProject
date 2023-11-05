
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameHelper.generated.h"

class AUnit;

UCLASS()
class SPROJECT_API UGameHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    AUnit* SpawnUnit() { return nullptr; }

};
