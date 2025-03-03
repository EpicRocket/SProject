
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

	bool operator==(const FUserAccount& Other) const
	{
		return Tie(ID, Token) == Tie(Other.ID, Other.Token);
	}

	bool operator<(const FUserAccount& Other) const
	{
		return Tie(ID, Token) < Tie(Other.ID, Other.Token);
	}
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
	int32 Level = INDEX_NONE;

	bool operator==(const FUserInfo& Other) const
	{
		return Tie(Name, Tag, Level) == Tie(Other.Name, Other.Tag, Other.Level);
	}

	bool operator<(const FUserInfo& Other) const
	{
		return Tie(Name, Tag, Level) < Tie(Other.Name, Other.Tag, Other.Level);
	}
};
