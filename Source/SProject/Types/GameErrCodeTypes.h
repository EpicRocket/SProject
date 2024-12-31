
#pragma once

#include "CoreMinimal.h"

#include "GameErrCodeTypes.generated.h"

UENUM()
enum class EGameErrCode : int32
{
	None = 0,

	// 엔진 에러
	NotFoundLocalPlayer,

	// 게임 에러
	UserDocumentUnpatchable,

	// 싱글플레이
	NotFoundSignleplaySubsystem,
	NotFoundSingleplayContext,

	Unkwon,
	Max
};
