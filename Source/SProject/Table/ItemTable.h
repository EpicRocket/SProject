// File generate 2024.01.14-14.49.17
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTable.generated.h"

USTRUCT(BlueprintType)
struct SPROJECT_API FItemInfoTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 kind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UseType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString icon;


};
