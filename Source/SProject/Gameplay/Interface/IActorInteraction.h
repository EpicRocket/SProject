
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "寃뚯엫?뚮젅???≫꽣|?곹샇?묒슜")
	void Interact();
	virtual void Interact_Implementation() {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "寃뚯엫?뚮젅???≫꽣|?곹샇?묒슜")
	void InteractWithActor(AActor* Instigator);
	virtual void InteractWithActor_Implementation(AActor* Instigator) {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "寃뚯엫?뚮젅???≫꽣|?곹샇?묒슜")
	void InteractWithPlayer(APlayerController* Instigator);
	virtual void InteractWithPlayer_Implementation(APlayerController* Instigator) {}
};
