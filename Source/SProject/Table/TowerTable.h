// This is an automatically generated file. Do not modify it manually. [2024.10.12-19.39.27]
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "TowerTable.generated.h"

enum class EAttackType : uint8;
class UTexture2D;

USTRUCT(BlueprintType)
struct SPROJECT_API FNormalTowerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Grade = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defend = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ObjectRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Splash = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SplashScale = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> IconPath = nullptr;
};

