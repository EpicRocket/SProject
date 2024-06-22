// File generate
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "BuildingTable.generated.h"

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
	None = 0,
	CommandCenter = 1,
	TowerEngineeringInstitute = 2,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EBuildingType, EBuildingType::Max)

USTRUCT(BlueprintType)
struct SPROJECT_API FBuildingTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuildingType Type = EBuildingType::Max;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
};

