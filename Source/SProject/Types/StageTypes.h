
#pragma once

#include "CoreMinimal.h"
#include "UnitTypes.h"

#include "StageTypes.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStageFlags : uint8
{
	None = 0						UMETA(DisplayName = "Nothing"),

	New			= 0x00000001		UMETA(DisplayName = "새로 시작"),
	Playing		= 0x00000002		UMETA(DisplayName = "진행 중"),
	Defeat		= 0x00000004		UMETA(DisplayName = "패배"),
	Clear		= 0x00000008		UMETA(DisplayName = "클리어"),
	Completed	= 0x00000016		UMETA(DisplayName = "완료됨"),
};
ENUM_CLASS_FLAGS(EStageFlags);

USTRUCT(BlueprintType)
struct MY_API FStage
{
	GENERATED_BODY()

	bool operator==(const FStage& Other) const
	{
		return Tie(Level, Towers) == Tie(Other.Level, Other.Towers);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/SProject.EStageFlags"))
	uint8 Flags = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Wave = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTower> Towers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UsePoint = INDEX_NONE;

};
