// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GGameCoreHelper.generated.h"

class APlayerController;
class ULocalPlayer;

UCLASS()
class GGAMECORE_API UGGameCoreBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetPrimaryPlayerController(UObject* WorldContextObject);

};
