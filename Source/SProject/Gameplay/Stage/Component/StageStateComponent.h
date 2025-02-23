
#pragma once

#include "Core/Component/GGameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

struct FLatentActionInfo;
struct FStage;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UStageStateComponent : public UGGameStateComponent, public IGLoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	
	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

	UFUNCTION(BlueprintCallable)
	FGErrorInfo LoadStage(const FStage& Stage);

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo WaitForPrimaryPlayerController(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStage(const FStage& Stage, const TSoftObjectPtr<class UWorld>& Level);

	UFUNCTION(BlueprintCallable)
	void SetTargetLevel(class AMyGameLevel* Level);

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> TargetStage;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bLoadCompleted = false;
};
