
#pragma once

#include "CoreMinimal.h"

#include "Resource.generated.h"


/** 아이템 정보 */
USTRUCT(BlueprintType)
struct SPROJECT_API FItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Key = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    auto operator<=>(const FItem&) const = default;
};
