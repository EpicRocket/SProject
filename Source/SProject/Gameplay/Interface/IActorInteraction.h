
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IActorInteraction.generated.h"

class AActor;
class AGPlayerController;

UINTERFACE(BlueprintType)
class MY_API UActorInteraction : public UInterface
{
	GENERATED_BODY()
};

class MY_API IActorInteraction : public IInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Actor|Interaction")
	void Interact();
	virtual void Interact_Implementation() {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Actor|Interaction")
	void InteractWithActor(AActor* Instigator);
	virtual void InteractWithActor_Implementation(AActor* Instigator) {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay|Actor|Interaction")
	void InteractWithPlayer(AGPlayerController* Instigator);
	virtual void InteractWithPlayer_Implementation(AGPlayerController* Instigator) {}
};
