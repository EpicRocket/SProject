
#pragma once

#include "CoreMinimal.h"
// include Project
#include "UserTypes.h"
#include "ResourceTypes.h"
#include "StageTypes.h"

#include "DocumentTypes.generated.h"

/** 기본적으로 사용자에게 필요한 정보 그룹 */
USTRUCT(BlueprintType)
struct MY_API FUserDocument
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserInfo UserInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Gold = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cash = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItem> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LastStageLevel = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStage> Stages;
};
