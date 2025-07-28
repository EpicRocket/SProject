// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "GameplayStageTypes.generated.h"

enum class EAttackType : uint8;
class UTexture2D;
class UBehaviorTree;
class AStageMonsterUnit;
class AStageTowerUnit;

// internal declare
class UStageWaveContext;
class UStageMonsterContext;
class UStageTowerContext;

USTRUCT(BlueprintType)
struct MY_API FStageInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> RewardItemKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsePoint = 0;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> IconPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;
};

UCLASS()
class MY_API UStageContext : public UGContext
{
	GENERATED_BODY()

public:
	virtual void Load() override;

public:
	UPROPERTY(BlueprintReadOnly)
	FStageInfo StageInfo;

	UPROPERTY(BlueprintReadOnly)
	TArray<UStageWaveContext*> WaveContexts;

	UPROPERTY(BlueprintReadOnly)
	TArray<UStageWaveContext*> RepaetWaveContexts;

	UPROPERTY(BlueprintReadOnly)
	TArray<UStageTowerContext*> TowerContexts;

};

UENUM(BlueprintType)
enum class EStageWaveType : uint8
{
	None	= 0 UMETA(Hidden),
	Default = 1 UMETA(DisplayName = "기본 모드"),
	Repeat	= 2 UMETA(DisplayName = "반복 모드"),
};

USTRUCT(BlueprintType)
struct MY_API FStageWaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimespan DelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterGroup = INDEX_NONE;

};

USTRUCT(BlueprintType)
struct MY_API FStageMonsterGroupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountValue = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Position = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimespan AmountDelayTime = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStageMonsterContext* MonsterContext = nullptr;

};

UCLASS()
class MY_API UStageWaveContext : public UGContext
{
	GENERATED_BODY()

public:
	virtual void Load() override;

public:
	UPROPERTY(BlueprintReadOnly)
	FStageWaveInfo WaveInfo;

	UPROPERTY(BlueprintReadOnly)
	TArray<FStageMonsterGroupInfo> MonsterGroupInfos;

};

USTRUCT(BlueprintType)
struct MY_API FStageMonsterInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Grade = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY()
	TSoftClassPtr<AStageMonsterUnit> UnitClassPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AStageMonsterUnit> UnitClass;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> IconPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY()
	TSoftObjectPtr<UBehaviorTree> AIPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> AI;

	bool operator==(const FStageMonsterInfo& Other) const {
		return Index == Other.Index
			&& Level == Other.Level
			&& Grade == Other.Grade;
	}

	FString ToString() const {
		return FString::Printf(
			TEXT("[Name:%s][Index:%d][Level:%d][Grade:%d]")
			, *Name.ToString()
			, Index
			, Level
			, Grade
		);
	}
};

UCLASS()
class MY_API UStageMonsterContext : public UGContext
{
	GENERATED_BODY()

public:
	virtual void Load() override;

public:
	UPROPERTY(BlueprintReadOnly)
	FStageMonsterInfo MonsterInfo;

};

UENUM(BlueprintType)
enum class EStageTowerType : uint8
{
	Normal = 0 UMETA(DisplayName = "일반"),
};

USTRUCT(BlueprintType)
struct MY_API FStageTowerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStageTowerType TowerType = EStageTowerType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UsePoint = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = static_cast<EAttackType>(0);

	UPROPERTY()
	TSoftClassPtr<AStageTowerUnit> UnitClassPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AStageTowerUnit> UnitClass;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> IconPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY()
	TSoftObjectPtr<UBehaviorTree> AIPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> AI;

	bool operator==(const FStageTowerInfo& Other) const {
		return TowerType == Other.TowerType
			&& Index == Other.Index
			&& Kind == Other.Kind
			&& Level == Other.Level;
	}

	FString ToString() const {
		return FString::Printf(
			TEXT("[Name:%s][Type:%s][Index:%d][Kind:%d][Level:%d]")
			, *Name.ToString()
			, *UEnum::GetValueAsString(TowerType)
			, Index
			, Kind
			, Level
		);
	}
};

UCLASS()
class MY_API UStageTowerContext : public UGContext
{
	GENERATED_BODY()

public:
	virtual void Load() override;

public:
	UPROPERTY(BlueprintReadOnly)
	FStageTowerInfo TowerInfo;

};


USTRUCT(BlueprintType)
struct FStageTowerReceipt
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStageTowerInfo> BuildTowers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMaxLevel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSellable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SellPrice = INDEX_NONE;
};
