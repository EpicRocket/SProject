
#pragma once

#include "Core/GGameInstance.h"

#include "MyGameInstance.generated.h"

UCLASS()
class MY_API UMyGameInstance : public UGGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

};
