// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "CommonInputBaseTypes.h"
#include "SuperGameUserSettings.generated.h"

struct FLoadedMappableConfigPair;
class UPlayerMappableInputConfig;
/**
 * 
 */
UCLASS()
class USuperGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	USuperGameUserSettings();

	static USuperGameUserSettings* Get();
};
