
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"

#include "GTeamTypes.generated.h"

class UGTeamDisplayAsset;

UENUM(BlueprintType, Category = "Team")
enum class ETeamRelationship : uint8
{
    Neutral     UMETA(DisplayName = "중립", ToolTip = "적대적 관계가 아니라서 기본적으로 공격 대상 취급을 하지 않습니다."),
    Friend      UMETA(DisplayName = "우호적"),
    Enemy       UMETA(DisplayName = "적대적"),
    Max         UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ETeamRelationship, ETeamRelationship::Max);

USTRUCT(BlueprintType, Category = "Team")
struct GGAMECORE_API FGTeamTracker : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 TeamID = 255;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UGTeamDisplayAsset> DisplayAssetPtr;
};
