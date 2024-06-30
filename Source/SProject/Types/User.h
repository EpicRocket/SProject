
#pragma once

#include "CoreMinimal.h"

#include "User.generated.h"


/** 유저 정보 */
USTRUCT(BlueprintType)
struct SPROJECT_API FUserInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Tag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;
};
