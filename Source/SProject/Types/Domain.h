
#pragma once

#include "CoreMinimal.h"

#include "Domain.generated.h"

/** 영지 건물 정보 */
USTRUCT(BlueprintType)
struct SPROJECT_API FDomainBuilding
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Key = 0;
};
