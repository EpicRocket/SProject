// This is an automatically generated file. Do not modify it manually. [2025.01.26-19.22.55]
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ConstTable.generated.h"



UCLASS(Config=SProject)
class SPROJECT_API UConstSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	int32 UsePointKey;

	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	int32 GoldKey;

	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	int32 UserHp;

	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	int32 StageUsePoint;

	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	int32 DefensiveConst;

	UPROPERTY(Config, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Table")
	float SpawnDelay;
};

