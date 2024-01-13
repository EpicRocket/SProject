
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "TableAsset.generated.h"

class UDataTable;

UCLASS()
class SPROJECT_API UTableAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
    TMap<FString, TObjectPtr<UDataTable>> Tables;
};
