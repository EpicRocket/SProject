
#pragma once

#include "Error/GErrorTypes.h"

#include "StageTableError.generated.h"

USTRUCT(BlueprintType)
struct FStageTableError : public FGErrorInfo
{
    GENERATED_BODY()

    FStageTableError() = default;
    FStageTableError(int32 Key)
    {
		ErrType = EGErrType::Error;
		ErrCode = TEXT("STAGE_TABLE_ERROR");
		Description = FText::Format(FText::FromString(TEXT("Stage에서 {0}번 데이터를 찾을 수 없습니다.")), Key);
    }
};
