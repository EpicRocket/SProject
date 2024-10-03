
#pragma once

#include "Components/GameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

class UStageDataAsset;
class AStageLevel;
class UWorld;
class APlayerController;
struct FGErrorInfo;
struct FLatentActionInfo;

UCLASS()
class MY_API UStageStateComponent : public UGameStateComponent, public IGLoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	
	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

public:
	UFUNCTION(BlueprintCallable)
	FGErrorInfo SetLevel(int32 Level);

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo OnLoadStage(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStreaming();

	UFUNCTION(BlueprintCallable)
	void FailLoadLevel();

	UFUNCTION(BlueprintCallable)
	void SuccessLoadLevel();

	UFUNCTION()
	void OnLoadLevelCompleted();
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnLoadCompleted"))
	void K2_OnLoadLevelCompleted();

public:
	UPROPERTY(EditDefaultsOnly, Category = "에셋")
	TSoftObjectPtr<UStageDataAsset> StageDataAsset;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStageLevel> TargetStage;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LoadLevel;

	// 로딩 완료 여부 테스트를 위해 우선 UPROPERTY로 뺌
	UPROPERTY(Transient, BlueprintReadWrite)
	bool bLoadCompleted = false;
};
