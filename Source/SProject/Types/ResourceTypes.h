
#pragma once

#include "CoreMinimal.h"

#include "ResourceTypes.generated.h"

/** ?꾩씠???뺣낫 */
USTRUCT(BlueprintType)
struct MY_API FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = INDEX_NONE;

	bool operator==(const FItem& Other) const
	{
		return Tie(Key, Count) == Tie(Other.Key, Other.Count);
	}

	bool operator<(const FItem& Other) const
	{
		return Tie(Key, Count) < Tie(Other.Key, Other.Count);
	}
};
