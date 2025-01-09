
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"

#include "GTeamTypes.generated.h"

class UGTeamDisplayAsset;

USTRUCT(BlueprintType)
struct GGAMECORE_API FGTeamTracker : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ID = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Force = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UGTeamDisplayAsset> DisplayAssetPtr;
};

USTRUCT(BlueprintType)
struct GGAMECORE_API FGTeamRelationship
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TrustLevel = 0;
};

USTRUCT(BlueprintType)
struct GGAMECORE_API FGTeam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGTeamTracker Tracker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<uint8/*TeamIndex*/, FGTeamRelationship> Relationships;
};
