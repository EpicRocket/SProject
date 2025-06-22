// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Core/Component/GGameStateComponent.h"
// include Engine
#include "UObject/SoftObjectPtr.h"
#include "Misc/EnumClassFlags.h"
// include GameCore
#include "Loading/Interface/IGLoadingProcess.h"
#include "Gameplay/Interface/IGameplayDataLoader.h"

#include "StageStateComponent.generated.h"

struct FLatentActionInfo;
struct FStage;
class AGameplayLevel;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStageLoadFlags : uint8
{
	None = 0,

	World = 0x00000001,
	Repository = 0x00000002,

	All				
	= World
	| Repository
	UMETA(Hidden),

	Complete = 0x000000FF UMETA(Hidden),	// 255
};
ENUM_CLASS_FLAGS(EStageLoadFlags);

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), ClassGroup = "Stage")
class MY_API UStageStateComponent : public UGGameStateComponent, public IGLoadingProcess, public IGameplayDataLoader
{
	GENERATED_BODY()

public:
	// UActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	// ~UActorComponent

	// IGLoadingProcess
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~IGLoadingProcess

	UFUNCTION(BlueprintCallable)
	FGErrorInfo LoadStage(const FStage& Stage);

	UFUNCTION(BlueprintCallable)
	void AddStageLoadFlags(EStageLoadFlags Flags, FGErrorInfo Error);

protected:
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"))
	FGErrorInfo WaitForPrimaryPlayerController(FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadStage(const FStage& Stage, const TSoftObjectPtr<class UWorld>& Level);

	UFUNCTION(BlueprintCallable)
	FGErrorInfo SetStageLevel(const FStage& Stage, AGameplayLevel* GameplayLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLoadComplete();

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class APlayerController> PrimaryPC;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AStageLevel> TargetStage;

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bLoadCompleted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFeature", meta = (AllowedTypes = "GameplayDataAsset"))
	FPrimaryAssetId GameplayDataAssetReference;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "GameFeature")
	TObjectPtr<const class UGameplayDataAsset> GameplayDataAsset;

private:
	EStageLoadFlags StageLoadFlags = EStageLoadFlags::None;

};
