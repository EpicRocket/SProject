
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GGameCoreHelper.generated.h"

class APlayerController;

UCLASS()
class GGAMECORE_API UGGameCoreBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetPrimaryPlayerController(UObject* WorldContextObject);
};
