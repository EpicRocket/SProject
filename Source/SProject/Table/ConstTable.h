// ?????뵬?? ?癒?짗??곗쨮 ??밴쉐?????뵬??낅빍?? ??롫짗??곗쨮 ??륁젟??? 筌띾뜆苑??

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

