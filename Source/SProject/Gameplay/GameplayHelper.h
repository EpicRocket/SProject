
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayHelper.generated.h"

UCLASS()
class MY_API UGameplayHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅??)
    static FGErrorInfo SetGameplayTagByInt32(AActor* Actor, FName TagName, int32 TagValue);

    UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅??)
    static FGErrorInfo GetGameplayTagByInt32(AActor* Actor, FName TagName, int32& TagValue);

	UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅??)
    static bool IsFarFrom(const FVector& A, const FVector& B, float SourceRadius = 1.0F);

};
