//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarTeamComponent.h"

uint8 UMortarTeamComponent::NeutralTeamID = 0;
// Sets default values for this component's properties
UMortarTeamComponent::UMortarTeamComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DefaultTeamInfo.TeamColor = FLinearColor(1, 1, 1);
	//Initially whiel spawning Team info would be default
	TeamInfo = DefaultTeamInfo;

}

//Gets Attitude based on the team ID
EMortarAgentAttitude::Type UMortarTeamComponent::GetAttitude(const uint8& ATeamID, const uint8& BTeamID)
{
	EMortarAgentAttitude::Type Attitude = EMortarAgentAttitude::Neutral;

	if (ATeamID != NeutralTeamID && BTeamID != NeutralTeamID)
	{
		if (ATeamID == BTeamID)
		{
			Attitude = EMortarAgentAttitude::Friendly;
		}
		else
		{
			Attitude = EMortarAgentAttitude::Hostile;
		}
	}

	return Attitude;
}


//Allow switching only if current id is neutral team id
bool UMortarTeamComponent::DoSwitchTeam(const FMortarTeamProperties& DesiredTeam)
{
	if (TeamInfo.TeamID == NeutralTeamID && TeamInfo.TeamID != DesiredTeam.TeamID)
	{
		UE_LOG(LogMortarPlugin, Log, TEXT("Team Switched From %s->%s"),*TeamInfo.TeamName,*DesiredTeam.TeamName);
		TeamInfo = DesiredTeam;
		//Broad Cast event so handling parties could handle accordingly
		OnTeamChange.Broadcast();
		return true;
	}

	return false;
}

void UMortarTeamComponent::SetTeamInfo(const FMortarTeamProperties& DesiredTeam)
{
	TeamInfo = DesiredTeam;
}


void UMortarTeamComponent::ResetTeamInfo()
{
	TeamInfo = DefaultTeamInfo;
}

FMortarTeamProperties UMortarTeamComponent::GetTeamInfo() const
{
	return TeamInfo;
}

void UMortarTeamComponent::SetDefaultTeamInfo(const FMortarTeamProperties& DesiredTeam)
{
	DefaultTeamInfo = DesiredTeam;
}

FMortarTeamProperties UMortarTeamComponent::GetDefaultTeamInfo() const
{
	return DefaultTeamInfo;
}
