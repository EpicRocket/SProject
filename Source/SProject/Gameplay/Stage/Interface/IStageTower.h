
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IStageTower.generated.h"

enum class EStageTowerType : uint8;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UStageTower : public UInterface
{
    GENERATED_BODY()
};

class MY_API IStageTower : public IInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "타워", meta = (ShortToolTip = "타워 타입"))
    virtual EStageTowerType GetTowerType() const = 0;

    UFUNCTION(BlueprintCallable, Category = "타워", meta = (ShortToolTip = "타워 분류"))
	virtual int32 GetKind() const = 0;

	UFUNCTION(BlueprintCallable, Category = "타워", meta = (ShortToolTip = "타워 레벨"))
	virtual int32 GetLevel() const = 0;

    UFUNCTION(BlueprintCallable, Category = "타워", meta = (ShortToolTip = "판매 가격"))
	virtual int64 GetSellPrice() const = 0;
};
