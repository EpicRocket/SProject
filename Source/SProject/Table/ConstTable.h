// This is an automatically generated file. Do not modify it manually. [2024.10.13-18.26.33]
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
	int32 UserHp;
};
