
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Math/BigInt.h"

#include "InventorySubsystem.generated.h"

struct FItem;

/*
 * 유저의 자원을 관리하는 시스템
 */
UCLASS()
class SPROJECT_API UInventorySubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "인벤토리")
    int64 GetGold() const;

    UFUNCTION(BlueprintCallable, Category = "인벤토리")
    int32 GetCash() const;

private:
    int64 Gold = 0;

    int32 Cash = 0;

    TMap<int32, TSharedPtr<FItem>> Items;
};
