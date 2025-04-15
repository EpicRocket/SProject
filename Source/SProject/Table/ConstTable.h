// 이 파일은 자동으로 생성된 파일입니다. 수동으로 수정하지 마세요.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "ConstTable.generated.h"


UCLASS(config = SProject, defaultconfig)
class SPROJECT_API UConstSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 UsePointKey;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 GoldKey;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 UserHp;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 StageUsePoint;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	int32 DefensiveConst;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Table")
	float SpawnDelay;

};

