﻿
#pragma once

#include "Core/MyPlayerController.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StagePlayerController.generated.h"

UENUM(BlueprintType)
enum class EStagePlayerControllerMode : uint8
{
    None                UMETA(DisplayName = "None"),
    PlayMode            UMETA(DisplayName = "플레이모드"),
    TowerBuildMode      UMETA(DisplayName = "타워건설모드"),
};

UCLASS(Abstract)
class MY_API AStagePlayerController : public AMyPlayerController
{
    GENERATED_BODY()

public:

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EStagePlayerControllerMode Mode = EStagePlayerControllerMode::None;
};

UCLASS()
class MY_API UStagePlayerControllerHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "스테이지", meta = (CompactNodeTitle = "StagePC", DefaultToSelf = "UserWidget"))
    static AStagePlayerController* GetStagePlayerControllerToWidget(UUserWidget* UserWidget);
};
