
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "StageTowerTypes.generated.h"

enum class EAttackType : uint8;
class UTexture2D;
class AStageTowerUnit;
class UBehaviorTree;

UENUM(BlueprintType)
enum class EStageTowerType : uint8
{
    Normal = 0 UMETA(DisplayName = "?쇰컲"),
};

USTRUCT(BlueprintType)
struct MY_API FStageTowerInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStageTowerType TowerType = EStageTowerType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Kind = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UsePoint = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAttackType AttackType = static_cast<EAttackType>(0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AStageTowerUnit> UnitClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UBehaviorTree> AI;

};

USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FStageTowerInfo> BuildTowers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMaxLevel = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSellable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 SellPrice = INDEX_NONE;
};
