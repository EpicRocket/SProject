
#pragma once

#include "CoreMinimal.h"

#include "DomainTypes.generated.h"

/** 영지 건물 정보 */
USTRUCT(BlueprintType)
struct MY_API FDomainBuilding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key = 0;

	bool operator==(const FDomainBuilding& Other) const
	{
	    return Tie(Key) == Tie(Other.Key);
	}

	bool operator<(const FDomainBuilding& Other) const
	{
		return Tie(Key) < Tie(Other.Key);
	}
};
