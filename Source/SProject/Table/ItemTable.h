// This is an automatically generated file. Do not modify it manually. [2024.10.27-18.47.37]
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTable.generated.h"

enum class EItemType : uint8;
enum class EUseType : uint8;


USTRUCT(BlueprintType)
struct SPROJECT_API FItemInfoTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType = static_cast<EItemType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUseType UseType = static_cast<EUseType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Value;
};

