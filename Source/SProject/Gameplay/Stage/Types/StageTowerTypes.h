
#pragma once

#include "CoreMinimal.h"
#include "Error/GErrorTypes.h"

#include "StageTowerTypes.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EStageTowerType : uint8
{
    Normal = 0 UMETA(DisplayName = "일반"),
};

USTRUCT(BlueprintType)
struct MY_API FBuildStageTower
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStageTowerType TowerType = EStageTowerType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Kind = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBuildStageTower> BuildTowers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGErrorInfo Error;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMaxLevel = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSellable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 SellPrice = 0;
};
