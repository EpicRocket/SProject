// This is an automatically generated file. Do not modify it manually.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "StageInfoTable.generated.h"

class UTexture2D;
class UWorld;

USTRUCT(BlueprintType)
struct SPROJECT_API FStageTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveGroup = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> StageIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Reward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> Map = nullptr;
};

