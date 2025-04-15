
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayHelper.generated.h"

UCLASS()
class MY_API UGameplayHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FGErrorInfo SetGameplayTagByInt32(AActor* Actor, FName TagName, int32 TagValue);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetGameplayTagByInt32(AActor* Actor, FName TagName, int32& TagValue);

	UFUNCTION(BlueprintPure)
	static bool IsFarFrom(const FVector& A, const FVector& B, float SourceRadius = 1.0F);

};
