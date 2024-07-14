
#pragma once

#include "Engine/DeveloperSettings.h"
#include "Types/DocumentTypes.h"

#include "SingleplaySettings.generated.h"


UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "싱글플레이"))
class SPROJECT_API USingleplaySettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere)
    FUserDocument UserDocument;

};
