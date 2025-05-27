// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/DataAsset.h"
#include "UObject/ScriptInterface.h"
#include "NativeGameplayTags.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Team/Interface/IGTeamAgent.h"

#include "StageBuildZone.generated.h"

enum class EStageTowerType : uint8;
struct FStageTowerInfo;
struct FStageTowerReceipt;
class UBoxComponent;
class AAIController;
class AStageLevel;
class AStageTowerUnit;

namespace Stage
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Selected_BuildZone);
}

USTRUCT(BlueprintType)
struct FSelectedStageBuildZoneMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AStageBuildZone> SelectedBuildZone;
};

USTRUCT(BlueprintType)
struct MY_API FStageBuildContent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStageTowerType TowerType = static_cast<EStageTowerType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;
};

UCLASS(BlueprintType, ClassGroup = "Stage")
class MY_API UStageBuildZoneData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStageBuildContent> BuildContents;
};

UCLASS(Abstract, BlueprintType, Blueprintable, ClassGroup = "Stage")
class MY_API AStageBuildZone : public ASkeletalMeshActor, public IGameplayActorTag, public IGTeamAgent
{
	GENERATED_BODY()

	static FName InteractionComponentName;

public:
	// IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~IGTeamAgent

public:
	AStageBuildZone();

	UFUNCTION(BlueprintPure)
	FStageTowerReceipt GetTowerReceipt() const;

	UFUNCTION(BlueprintCallable)
	void RequestBuildTower(const FStageTowerInfo& BuildStageTower);

	UFUNCTION(BlueprintCallable)
	void RequestDemolishTower(const int64 SellPrice);

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetBuildLocation() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnCompleteBuildTower(AStageTowerUnit* Tower);

private:
	void AddUsePoint(int64 Point);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStageBuildZoneData> BuildZoneData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AStageLevel> SourceStage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AStageTowerUnit> SpawnedTower;

	UPROPERTY(EditInstanceOnly, Category = "Team")
	uint8 TeamID = 255;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionComponent;

};
