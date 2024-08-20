
#pragma once

#include "GameFramework/MyPlayerController.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "StagePlayerController.generated.h"

struct FTowerTableRow;

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
    UFUNCTION(BlueprintCallable, Category = "빌드", meta = (ShortTooltip = "모든 건설 타워 데이터를 가져 옵니다."))
    bool GetBuildTowerDatas(TArray<FTowerTableRow>& OutDatas) const;

    UFUNCTION(BlueprintCallable, Category = "빌드", meta = (ShortTooltip = "유저가 건설 가능한 건설 타워 데이터들을 가져 옵니다."))
    bool GetBuildTowerDatasForUser(TArray<FTowerTableRow>& OutDatas) const;

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
