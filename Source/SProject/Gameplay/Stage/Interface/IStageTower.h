// Copyright (c) 2025 Team EpicRocket. All rights reserved.

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
	UFUNCTION(BlueprintCallable)
	virtual EStageTowerType GetTowerType() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetKind() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetLevel() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual int64 GetSellPrice() const = 0;
};
