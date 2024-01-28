
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemTable.generated.h"

USTRUCT(BlueprintType)
struct SPROJECT_API FItemInfoTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 Index = INDEX_NONE;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 Kind = INDEX_NONE;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString ItemType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString UseType;
};
