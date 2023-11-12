
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

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    AUnit* SummonUnitFromGameStage(const UObject* WorldContextObject, int32 GameStageId, int32 UnitId);

};
