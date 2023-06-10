
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "XlsxObject.generated.h"

UCLASS(BlueprintType)
class UXlsxObject : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<TObjectPtr<UObject>> Structs;

    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<TObjectPtr<UObject>> Enums;

    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<TObjectPtr<class UDataTable>> DataTables;
};
