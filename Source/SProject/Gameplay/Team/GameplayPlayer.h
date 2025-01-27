
#pragma once

#include "CoreMinimal.h"

#include "GameplayPlayer.generated.h"

struct FGErrorInfo;

UCLASS(Abstract, BlueprintType)
class MY_API UGameplayPlayer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	virtual int64 GetItemCount(int32 Key) const;

	UFUNCTION(BlueprintCallable)
	virtual FGErrorInfo ConsumeItem(int32 Key, int32 Count);

	UFUNCTION(BlueprintPure)
	int64 GetUsePoint() const;

	UFUNCTION(BlueprintPure)
	FGErrorInfo ConsumeUsePoint(int32 Count);

	UFUNCTION(BlueprintPure)
	int64 GetGold() const;

	UFUNCTION(BlueprintPure)
	FGErrorInfo ConsumedGold(int32 Count);

public:

};
