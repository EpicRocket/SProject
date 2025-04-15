
#pragma once

#include "Team/Subsystem/GTeamSubsystem.h"

#include "GameplayTeamSubsystem.generated.h"

class UDataTable;
class AGameplayPlayer;
class APlayerController;

UCLASS()
class MY_API UGameplayTeamSubsystem : public UGTeamSubsystem
{
	GENERATED_BODY()

protected:
	virtual void OnRegisterTeams() override;
	virtual void OnUnregisterTeams() override;

public:
	UFUNCTION(BlueprintCallable)
	uint8 IssusePlayerTeamID(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	AGameplayPlayer* GetPlayer(uint8 ID) const;

private:
	UPROPERTY()
	TMap<uint8, AGameplayPlayer*> Players;
};
