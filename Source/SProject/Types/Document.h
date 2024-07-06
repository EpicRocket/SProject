
#pragma once

#include "CoreMinimal.h"
// include Project
#include "User.h"
#include "Resource.h"
#include "Domain.h"

#include "Document.generated.h"

/** 기본적으로 사용자에게 필요한 정보 그룹 */
USTRUCT()
struct FUserDocument
{
    GENERATED_BODY()

    UPROPERTY()
	FUserInfo UserInfo;

    UPROPERTY()
	int64 Gold = 0;

    UPROPERTY()
	int32 Cash = 0;

    UPROPERTY()
	TArray<FItem> Items;

    UPROPERTY()
	TArray<FDomainBuilding> DomainBuildings;
};
