﻿
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ISelectableActor.generated.h"

class UPlayerSelectComponent;

UINTERFACE(BlueprintType)
class MY_API USelectableActor : public UInterface
{
	GENERATED_BODY()
};

class MY_API ISelectableActor : public IInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|선택")
	bool IsSelectableActor() const;
	virtual bool IsSelectableActor_Implementation() const { return true; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|선택")
	void SelectActor(APlayerController* PC);
	virtual void SelectActor_Implementation(APlayerController* PC) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|선택")
	void DeselectActor(APlayerController* PC);
	virtual void DeselectActor_Implementation(APlayerController* PC) {};
};