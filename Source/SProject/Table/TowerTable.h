// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "TowerTable.generated.h"

class AStageTowerUnit;
class UBehaviorTree;
class UTexture2D;
enum class EAttackType : uint8;

USTRUCT(BlueprintType)
struct SPROJECT_API FNormalTowerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Grade = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ObjectRange = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Splash = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SplashScale = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AStageTowerUnit> Unit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UBehaviorTree> AI;

};

