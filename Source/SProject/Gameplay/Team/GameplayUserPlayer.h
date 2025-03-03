
#pragma once

#include "GameplayPlayer.h"

#include "GameplayUserPlayer.generated.h"

class APlayerController;
class ULocalPlayer;

UCLASS(BlueprintType)
class MY_API AGameplayUserPlayer : public AGameplayPlayer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	APlayerController* GetOwningPlayerController() const;

	UFUNCTION(BlueprintPure)
	ULocalPlayer* GetOwningLocalPlayer() const;


public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> OwningPlayerController;

};
