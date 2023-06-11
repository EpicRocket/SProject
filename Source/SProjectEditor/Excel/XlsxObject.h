
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
	FString SubsystemPath;

    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<UObject*> Structs;

    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<UObject*> Enums;

    UPROPERTY(VisibleAnywhere, Category = "XlsxObject")
    TArray<class UDataTable*> DataTables;
};
