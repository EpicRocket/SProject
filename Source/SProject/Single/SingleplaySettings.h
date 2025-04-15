
#pragma once

#include "Engine/DeveloperSettings.h"
#include "Types/DocumentTypes.h"

#include "SingleplaySettings.generated.h"


UCLASS(Config = Game, DefaultConfig)
class MY_API USingleplaySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	FUserDocument UserDocument;

};
