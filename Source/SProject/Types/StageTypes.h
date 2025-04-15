
#pragma once

#include "CoreMinimal.h"
#include "UnitTypes.h"

#include "StageTypes.generated.h"

USTRUCT(BlueprintType)
struct MY_API FStage
{
	GENERATED_BODY()

	bool operator==(const FStage& Other) const
	{
		return Tie(Level, Towers) == Tie(Other.Level, Other.Towers);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTower> Towers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UsePoint = INDEX_NONE;
};
