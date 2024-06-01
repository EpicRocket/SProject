
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "MyUIHelper.generated.h"

class ULocalPlayer;

UCLASS()
class SPROJECT_API UMyUIHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FName SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason);

    static void ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken);
};
