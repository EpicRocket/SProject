
#pragma once

#include "CoreMinimal.h"

#include "GameErrCodeTypes.generated.h"

UENUM()
enum class EGameErrCode : int32
{
	None = 0,

	// ?붿쭊 ?먮윭
	NotFoundLocalPlayer,

	// 寃뚯엫 ?먮윭
	UserDocumentUnpatchable,

	// ?깃??뚮젅??
	NotFoundSignleplaySubsystem,
	NotFoundSingleplayContext,

	Unkwon,
	Max
};
