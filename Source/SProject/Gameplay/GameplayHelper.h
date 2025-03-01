
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayHelper.generated.h"

UCLASS()
class MY_API UGameplayHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "게임플레이")
    static FGErrorInfo SetGameplayTagByInt32(AActor* Actor, FName TagName, int32 TagValue);

    UFUNCTION(BlueprintPure, Category = "게임플레이")
    static FGErrorInfo GetGameplayTagByInt32(AActor* Actor, FName TagName, int32& TagValue);

};
