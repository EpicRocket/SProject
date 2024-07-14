
#pragma once

#include "CoreMinimal.h"
// include Project
#include "UserTypes.h"
#include "ResourceTypes.h"
#include "DomainTypes.h"
#include "StageTypes.h"

#include "DocumentTypes.generated.h"


/** 기본적으로 사용자에게 필요한 정보 그룹 */
USTRUCT(BlueprintType)
struct SPROJECT_API FUserDocument
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserInfo UserInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Gold = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cash = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItem> Items;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDomainBuilding> DomainBuildings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FStage Stage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FStageDetail> StageDetails;
};
