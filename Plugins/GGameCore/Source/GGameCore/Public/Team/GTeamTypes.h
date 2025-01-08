
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"

#include "GTeamTypes.generated.h"

class UGTeamDisplayAsset;
namespace ETeamAttitude
{
    enum Type : int;
}

USTRUCT(BlueprintType)
struct FGTeamForce : public FTableRowBase
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETeamAttitude::Type> Attitude = TEnumAsByte<ETeamAttitude::Type>{ 0 };
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
    TSoftObjectPtr<UGTeamDisplayAsset> DisplayAssetPtr;
};
