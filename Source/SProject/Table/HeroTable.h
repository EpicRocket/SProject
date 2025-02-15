// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "HeroTable.generated.h"

class UTexture2D;
enum class EAttackType : uint8;

USTRUCT(BlueprintType)
struct SPROJECT_API FHeroSkillTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Name = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defend = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackSpeed = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReactionRange = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Splash = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SplashRange = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillDesc;

};

USTRUCT(BlueprintType)
struct SPROJECT_API FHeroTableRow : public FTableRowBase
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
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defend = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackSpeed = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReactionRange = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Splash = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SplashRange = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText HeroName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText HeroDesc;

};

