
#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Error/GErrorTypes.h"

#include "GErrorManager.generated.h"

UCLASS()
class GGAMECORE_API UGErrorManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:

private:
	UPROPERTY()
	TMap<FString, FGErrorInfo> Errs;
};

UCLASS()
class GGAMECORE_API UGErrorHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Error", meta = (ReturnDisplayName = "Passed"))
	static bool IsOk(const FGErrorInfo& Err);

	UFUNCTION(BlueprintPure, Category = "Error")
	static FGErrorInfo Pass();
};
