// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IStageActor.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UStageActor : public UInterface
{
	GENERATED_BODY()

};

class MY_API IStageActor : public IInterface
{
    GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable)
    //virtual void BindStage() = 0;
    
};
