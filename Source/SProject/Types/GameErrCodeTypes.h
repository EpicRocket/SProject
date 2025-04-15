
#pragma once

#include "CoreMinimal.h"

#include "GameErrCodeTypes.generated.h"

UENUM()
enum class EGameErrCode : int32
{
	None = 0,

	NotFoundLocalPlayer,

	UserDocumentUnpatchable,

	NotFoundSignleplaySubsystem,
	NotFoundSingleplayContext,

	Unkwon,
	Max
};
