
#pragma once

#include "Engine/DataAsset.h"

#include "GTeamDisplayAsset.generated.h"

UCLASS(BlueprintType, Category = "Team")
class GGAMECORE_API UGTeamDisplayAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;
};
