
#pragma once

#include "CoreMinimal.h"
#include "UnitTypes.h"

#include "StageTypes.generated.h"

/** 스테이지 정보 */
USTRUCT(BlueprintType)
struct MY_API FStage
{
	GENERATED_BODY()

	bool operator==(const FStage& Other) const
	{
		return Tie(Level, Towers) == Tie(Other.Level, Other.Towers);
	}

	/** 스테이지의 레벨(맵) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	/** 스테이지에 배치된 타워들 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTower> Towers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UsePoint = INDEX_NONE;
};
