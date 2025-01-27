
#pragma once

#include "Team/Subsystem/GTeamSubsystem.h"

#include "GameplayTeamSubsystem.generated.h"

class UDataTable;
class UGameplayPlayer;

UCLASS()
class MY_API UGameplayTeamSubsystem : public UGTeamSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
	UGameplayPlayer* GetPlayer(uint8 ID) const;

protected:
    virtual void OnRegisterTeams() override;

private:
    UPROPERTY()
	TMap<uint8, UGameplayPlayer*> Players;
};
