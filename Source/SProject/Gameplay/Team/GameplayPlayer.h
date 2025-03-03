
#pragma once

#include "GameFramework/Info.h"
#include "Team/Interface/IGTeamAgent.h"

#include "GameplayPlayer.generated.h"

struct FGErrorInfo;
struct FGTeam;
class UGameplayTeamSubsystem;
class UGameplayPlayerComponent;

UCLASS(Abstract, BlueprintType)
class MY_API AGameplayPlayer : public AInfo, public IGTeamAgent
{
	GENERATED_BODY()

	friend class UGameplayTeamSubsystem;

public:
	AGameplayPlayer();

	// IGTeamAgent
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~IGTeamAgent

public:
	TSharedRef<FGTeam> GetInfo() const;

public:
	UPROPERTY(EditInstanceOnly, Category = "Team")
	uint8 TeamID = 255;

private:
	TWeakPtr<FGTeam> InfoPtr;

};
