
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

	TOptional<int64> Gold;

	TOptional<int32> Cash;

	TOptional<TArray<FItem>> Items;

	TOptional<int32> LastStageLevel;

	TOptional<TArray<FStage>> Stages;
	// ~UserDocument
};
