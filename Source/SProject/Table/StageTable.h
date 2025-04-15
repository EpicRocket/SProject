// ?????뵬?? ?癒?짗??곗쨮 ??밴쉐?????뵬??낅빍?? ??롫짗??곗쨮 ??륁젟??? 筌띾뜆苑??

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "StageTable.generated.h"

class UTexture2D;
class UWorld;

USTRUCT(BlueprintType)
struct SPROJECT_API FStageTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveGroup = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Reward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> Map;

};

USTRUCT(BlueprintType)
struct SPROJECT_API FWaveTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Wave_Group = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Type = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Monster_Group = INDEX_NONE;

};

