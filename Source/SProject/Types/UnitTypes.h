
#pragma once

#include "CoreMinimal.h"

#include "UnitTypes.generated.h"


USTRUCT(BlueprintType)
struct MY_API FUnit
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct MY_API FTower
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Key = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Location = INDEX_NONE;

    auto operator<=>(const FTower&) const = default;
};
