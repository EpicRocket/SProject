
#pragma once

#include "CoreMinimal.h"

#include "UserTypes.generated.h"

/*
 * 지원하는 유저 로그인 타입
*/
UENUM(BlueprintType)
enum class EUserLoginType : uint8
{
    None = 0,
    Singleplay,
    Guest,
};

USTRUCT(BlueprintType)
struct MY_API FUserAccount
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Token;
};

/** 유저 정보 */
USTRUCT(BlueprintType)
struct MY_API FUserInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Tag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    auto operator<=>(const FUserInfo&) const = default;
};
