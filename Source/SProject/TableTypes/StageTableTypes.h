﻿
#pragma once

#include "CoreMinimal.h"

#include "StageTableTypes.generated.h"

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
