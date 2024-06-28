
#pragma once

#include "Core/GGameInstance.h"

#include "MyGameInstance.generated.h"

UCLASS()
class SPROJECT_API UMyGameInstance : public UGGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

};
