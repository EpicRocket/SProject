
#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "StageMonsterTypes.generated.h"

enum class EAttackType : uint8;
class UTexture2D;

USTRUCT(BlueprintType)
struct MY_API FStageMonsterInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Grade = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* UnitClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon;

};
