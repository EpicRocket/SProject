
#pragma once

#include "CoreMinimal.h"

#include "ResourceTypes.generated.h"

/** 아이템 정보 */
USTRUCT(BlueprintType)
struct MY_API FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	bool operator==(const FItem& Other) const
	{
		return Tie(Key, Count) == Tie(Other.Key, Other.Count);
	}

	bool operator<(const FItem& Other) const
	{
		return Tie(Key, Count) < Tie(Other.Key, Other.Count);
	}
};
