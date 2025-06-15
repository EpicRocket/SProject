// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "GameplayPlayer.h"

#include "GameplayUserPlayer.generated.h"

class APlayerController;
class ULocalPlayer;

UCLASS()
class GGAMECORE_API AGameplayUserPlayer : public AGameplayPlayer
{
	GENERATED_BODY()

protected:
	// AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~AActor

public:
	UFUNCTION(BlueprintPure)
	APlayerController* GetOwningPlayerController() const;

	UFUNCTION(BlueprintPure)
	ULocalPlayer* GetOwningLocalPlayer() const;

public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> OwningPlayerController;

};
