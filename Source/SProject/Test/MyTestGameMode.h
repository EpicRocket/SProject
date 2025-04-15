
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "MyTestGameMode.generated.h"

UCLASS(Config = Game, notplaceable, BlueprintType, Blueprintable, Transient, hideCategories = (Info, Rendering, MovementReplication, Replication, Actor))
class MY_API AMyTestGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

};
