
#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Engine/DataTable.h"

#include "PlayerTeam.generated.h"

UENUM(BlueprintType)
enum class EPlayerAgentAttitude
{
    Neutral = 0,
    Friendly = 1,
    Hostile = 2,
};

USTRUCT(BlueprintType)
struct FPlayerTeam : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGenericTeamId TeamID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPlayerAgentAttitude Attitude;
};
