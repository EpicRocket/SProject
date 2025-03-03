
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"

#include "GTeamTypes.generated.h"

class UGTeamDisplayAsset;

UENUM(BlueprintType)
enum class EGTeamPlayerType : uint8
{
	Player,
	Computer,
};

USTRUCT(BlueprintType)
struct GGAMECORE_API FGTeamTracker : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ID = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Force = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGTeamPlayerType PlayerType = EGTeamPlayerType::Player;

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

USTRUCT(BlueprintType)
struct GGAMECORE_API FGRelationshipInForceTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Force = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGTeamRelationship Relationship;

};

USTRUCT(BlueprintType)
struct GGAMECORE_API FGForcesRelationshipTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SourceForce = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DestForce = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGTeamRelationship Relationship;

};

