
#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
// include Project
#include "DocumentTypes.h"

#include "FetchDocumentTypes.generated.h"

USTRUCT()
struct MY_API FFetchDocument
{
	GENERATED_BODY()

	// UserDocument
	TOptional<FUserInfo> UserInfo;

	TOptional<int64> Gold = 0;

	TOptional<int32> Cash = 0;

	TOptional<TArray<FItem>> Items;

	TOptional<TArray<FDomainBuilding>> DomainBuildings;

	TOptional<FStage> Stage;

	TOptional<TArray<FStageDetail>> StageDetails;
	// ~UserDocument
};
