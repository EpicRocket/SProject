#pragma once
#include "GameFramework/GameModeBase.h"
#include "HousingGameMode.generated.h"


UCLASS()
class SPROJECT_API AHousingGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:

    virtual void PostLogin(APlayerController* NewPlayer) override;


protected:


};
