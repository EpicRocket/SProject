// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperGameUserSettings.h"

#include "PlayerMappableInputConfig.h"
#include "SuperMappableConfigPair.h"

USuperGameUserSettings::USuperGameUserSettings()
{
}

USuperGameUserSettings* USuperGameUserSettings::Get()
{
	return GEngine ? CastChecked<USuperGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}
