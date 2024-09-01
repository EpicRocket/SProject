// This is an automatically generated file. Do not modify it manually.
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "TowerTable.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct SPROJECT_API FTowerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
};

