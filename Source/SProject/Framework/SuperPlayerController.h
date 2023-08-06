// Copyright 2023. GreenTeaRocket all rights reserved.

#pragma once

#include "CommonPlayerController.h"
#include "SuperPlayerController.generated.h"

UCLASS()
class ASuperPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
    //~ Begin ACommonPlayerController
    virtual void BeginPlay() override;
    //~ End ACommonPlayerController

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<class UInputMappingContext> MappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
    int32 MappingPriority = 0;
};
