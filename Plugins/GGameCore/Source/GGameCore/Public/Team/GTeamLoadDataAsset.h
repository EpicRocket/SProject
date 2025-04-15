
#pragma once

#include "Engine/DataAsset.h"

#include "GTeamLoadDataAsset.generated.h"

class UDataTable;

UCLASS(BlueprintType, Category = "Team")
class GGAMECORE_API UGTeamLoadDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RequiredAssetDataTags="RowStructure=/Script/GGameCore.GTeamTracker"))
	UDataTable* TeamTracker;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RequiredAssetDataTags="RowStructure=/Script/GGameCore.GRelationshipInForceTableRow"))
	UDataTable* RelationshipInForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (RequiredAssetDataTags="RowStructure=/Script/GGameCore.GForcesRelationshipTableRow"))
	UDataTable* ForcesRelationship;

};
