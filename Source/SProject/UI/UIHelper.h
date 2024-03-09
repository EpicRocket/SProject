
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/SoftObjectPtr.h"

#include "UIHelper.generated.h"

class APlayerController;
class UCommonActivatableWidget;
class USubActivatableWidget;
class UGameLayout;
class UUserWidget;

struct FGameplayTag;
struct FGameplayTagContainer;

UCLASS()
class UUIHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FText FormatWithSuffix(int64 Value, int32 Digits = 2);
};
