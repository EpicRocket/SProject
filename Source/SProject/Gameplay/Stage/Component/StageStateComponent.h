
#pragma once

#include "Components/GameStateComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "Loading/Interface/GLoadingProcessInterface.h"

#include "StageStateComponent.generated.h"

class UStageDataAsset;
class AStageLevel;
class AMyGameLevel;
class UWorld;
class APlayerController;
struct FGErrorInfo;
struct FLatentActionInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UStageStateComponent : public UGameStateComponent, public IGLoadingProcessInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeComponent() override;
	
	// IGLoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcessInterface

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo OnLoadStage(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStageCompleted();

	UFUNCTION(BlueprintCallable)
	void SetTargetLevel(AMyGameLevel* Level);

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStageLevel> TargetStage;

protected:
	bool bLoadCompleted = false;
};
