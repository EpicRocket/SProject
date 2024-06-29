// File generate: 2024.06.30-02.55.36
#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "Engine/DataTable.h"
#include "DomainTable.generated.h"

enum class EDomainType : uint8;

UENUM(BlueprintType)
enum class EDomainType : uint8
{
	None = 0,
	CommandCenter = 1,
	TowerEngineeringInstitute = 2,
	Max UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EDomainType, EDomainType::Max)

USTRUCT(BlueprintType)
struct SPROJECT_API FDomainTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDomainType Type = static_cast<EDomainType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
};

