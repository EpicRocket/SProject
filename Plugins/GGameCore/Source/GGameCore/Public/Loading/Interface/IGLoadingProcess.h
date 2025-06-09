
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IGLoadingProcess.generated.h"

UINTERFACE(BlueprintType)
class GGAMECORE_API UGLoadingProcess : public UInterface
{
	GENERATED_BODY()
};

class GGAMECORE_API IGLoadingProcess : public IInterface
{
	GENERATED_BODY()

public:
	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const;
};
