
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/ScriptInterface.h"
#include "NativeGameplayTags.h"
#include "Gameplay/Interface/IGameplayActorTag.h"
#include "Gameplay/ETC/GameplayTeamActor.h"

#include "StageBuildZone.generated.h"

enum class EStageTowerType : uint8;
struct FBuildStageTower;
struct FStageTowerReceipt;
class UBoxComponent;
class AAIController;
class AUnitStageTower;

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

UCLASS(BlueprintType)
class MY_API UStageBuildZoneData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FStageBuildContent> BuildContents;
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AStageBuildZone : public AGameplayTeamActor, public IGameplayActorTag
{
	GENERATED_BODY()

	static FName InteractionComponentName;

public:
	AStageBuildZone();

	UFUNCTION(BlueprintPure)
	FStageTowerReceipt GetTowerReceipt() const;

	UFUNCTION(BlueprintCallable)
	void RequestBuildTower(const FBuildStageTower& BuildStageTower);

	UFUNCTION(BlueprintCallable)
	void RequestDemolishTower();

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetBuildLocation() const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStageBuildZoneData> BuildZoneData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AUnitStageTower> SpawnedTower;

private:
	UPROPERTY(Category = "스테이지", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionComponent;
};
