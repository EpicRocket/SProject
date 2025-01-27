
#include "GameplayTeamSubsystem.h"
// include GameCore
#include "Team/GTeamTypes.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Team/GameplayUserPlayer.h"
#include "Gameplay/Team/GameplayComputerPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTeamSubsystem)

UGameplayPlayer* UGameplayTeamSubsystem::GetPlayer(uint8 ID) const
{
	if (Players.Contains(ID))
	{
		return Players[ID];
	}
	UE_LOG(LogGameplay, Warning, TEXT("UGameplayPlayer을 찾을 수 없습니다. [TeamID: %d]"), ID);
	return nullptr;
}

void UGameplayTeamSubsystem::OnRegisterTeams()
{
	EObjectFlags NewFlags = EObjectFlags::RF_Transient | EObjectFlags::RF_Public;

	for (auto& [ID, Team] : Teams)
	{
		UGameplayPlayer* Player = nullptr;
		if (Team->Tracker.PlayerType == EGTeamPlayerType::Player)
		{
			Player = NewObject<UGameplayUserPlayer>(this, NAME_None, NewFlags);
		}
		else if (Team->Tracker.PlayerType == EGTeamPlayerType::Computer)
		{
			Player = NewObject<UGameplayComputerPlayer>(this, NAME_None, NewFlags);
		}
		else
		{
			UE_LOG(LogGameplay, Warning, TEXT("플레이어 타입이 지정되지 않았습니다. ID: %d"), ID);
			continue;
		}
		
		if (!Player)
		{
			UE_LOG(LogGameplay, Warning, TEXT("플레이어를 생성할 수 없습니다. ID: %d"), ID);
			continue;
		}

		Players.Emplace(ID, Player);
	}
}
 