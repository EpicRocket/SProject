// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TerritoryGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPROJECT_API ATerritoryGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATerritoryGameMode();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory", meta = (AllowPrivateAccess))
	float DoubleClickTime;

public:
	FORCEINLINE float GetDoubleClickTime() const { return DoubleClickTime; }
};