
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IActorInteraction.generated.h"

class AActor;
class APlayerController;

UINTERFACE(BlueprintType)
class MY_API UActorInteraction : public UInterface
{
	GENERATED_BODY()
};

class MY_API IActorInteraction : public IInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|상호작용")
	void Interact();
	virtual void Interact_Implementation() {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|상호작용")
	void InteractWithActor(AActor* Instigator);
	virtual void InteractWithActor_Implementation(AActor* Instigator) {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "게임플레이|액터|상호작용")
	void InteractWithPlayer(APlayerController* Instigator);
	virtual void InteractWithPlayer_Implementation(APlayerController* Instigator) {}
};
