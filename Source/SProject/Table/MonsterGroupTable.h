// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "MonsterGroupTable.generated.h"


USTRUCT(BlueprintType)
struct SPROJECT_API FMonsterGroupTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Group = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterKind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountValue = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Position = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountDelayTime = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnTime = INDEX_NONE;

};

