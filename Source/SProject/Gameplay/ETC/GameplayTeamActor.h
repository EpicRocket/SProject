
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Team/Interface/IGTeamAgent.h"

#include "GameplayTeamActor.generated.h"

class AGPlayerController;

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API AGameplayTeamActor : public AActor, public IGTeamAgent
{
	GENERATED_BODY()

public:
    // IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
    // ~IGTeamAgent

public:
    UPROPERTY(EditInstanceOnly, Category = "Team")
    uint8 TeamID;
};
