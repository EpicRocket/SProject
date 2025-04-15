
#pragma once

#include "CoreMinimal.h"

#include "GPhaseMessage.generated.h"

USTRUCT(BlueprintType)
struct GGAMECORE_API FGPhaseMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive = false;
};
