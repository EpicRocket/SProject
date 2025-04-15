
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GLoadingProcessInterface.generated.h"

UINTERFACE(BlueprintType)
class GGAMECORE_API UGLoadingProcessInterface : public UInterface
{
    GENERATED_BODY()
};

class GGAMECORE_API IGLoadingProcessInterface : public IInterface
{
    GENERATED_BODY()

public:
    static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

    virtual bool ShouldShowLoadingScreen(FString& OutReason) const;
};
