
#pragma once

#include "Components/GameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

class UStageDataAsset;
class AStageLevel;
struct FGErrorInfo;

UCLASS()
class MY_API UStageStateComponent : public UGameStateComponent, public IGLoadingProcessInterface
{
    GENERATED_BODY()

public:
    virtual void InitializeComponent() override;
    
    virtual void BeginPlay() override;

    // IGLoadingProcessInterface
    virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
    // ~IGLoadingProcessInterface

public:
	UFUNCTION(BlueprintCallable)
    FGErrorInfo SetLevel(int32 Level);

public:
    UPROPERTY(EditDefaultsOnly, Category = "에셋")
    TSoftObjectPtr<UStageDataAsset> StageDataAsset;

	UPROPERTY(Transient, BlueprintReadOnly)
    TWeakObjectPtr<AStageLevel> TargetStage;

private:
    bool bLoadCompleted = false;
};
