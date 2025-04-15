// ?????뵬?? ?癒?짗??곗쨮 ??밴쉐?????뵬??낅빍?? ??롫짗??곗쨮 ??륁젟??? 筌띾뜆苑??

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
	int32 Sort = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterKind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = INDEX_NONE;

};

