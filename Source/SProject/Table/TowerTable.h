// This is an automatically generated file. Do not modify it manually. [2024.12.01-17.03.22]
#pragma once

#include "CoreMinimal.h"
enum class EAttackType : uint8;
class UTexture2D;

#include "Engine/DataTable.h"
#include "TowerTable.generated.h"

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
	float AttackSpeed = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ObjectRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Splash = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SplashScale = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> IconPath = nullptr;
};

