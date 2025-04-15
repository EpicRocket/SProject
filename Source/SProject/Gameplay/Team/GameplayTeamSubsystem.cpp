
#include "GameplayTeamSubsystem.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
// include GameCore
#include "Team/GTeamTypes.h"
// include Project
#include "Gameplay/GameplayLogging.h"
#include "Gameplay/Team/GameplayUserPlayer.h"
#include "Gameplay/Team/GameplayComputerPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTeamSubsystem)

void UGameplayTeamSubsystem::OnRegisterTeams()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags = EObjectFlags::RF_Transient | EObjectFlags::RF_Public;

	for (auto& [ID, Team] : Teams)
	{
		AGameplayPlayer* Player = nullptr;
		if (Team->Tracker.PlayerType == EGTeamPlayerType::Player)
		{
			Player = GetWorld()->SpawnActor<AGameplayUserPlayer>(SpawnParams);
		}
		else if (Team->Tracker.PlayerType == EGTeamPlayerType::Computer)
		{
			Player = GetWorld()->SpawnActor<AGameplayComputerPlayer>(SpawnParams);
		}
		else
		{
			UE_LOG(LogGameplay, Warning, TEXT("?뚮젅?댁뼱 ??낆씠 吏?뺣릺吏 ?딆븯?듬땲?? ID: %d"), ID);
			continue;
		}

		if (!Player)
		{
			UE_LOG(LogGameplay, Warning, TEXT("?뚮젅?댁뼱瑜??앹꽦?????놁뒿?덈떎. ID: %d"), ID);
			continue;
		}

		Player->InfoPtr = Team;
		Player->SetGenericTeamId(ID);

		Players.Emplace(ID, Player);
	}
}

void UGameplayTeamSubsystem::OnUnregisterTeams()
{
	for (auto& [ID, Player] : Players)
	{
		Player->Destroy();
	}
	Players.Empty();
}

uint8 UGameplayTeamSubsystem::IssusePlayerTeamID(APlayerController* PC)
{
	for (auto& [ID, Player] : Players)
	{
		auto User = Cast<AGameplayUserPlayer>(Player);
		if (!IsValid(User))
		{
			continue;
		}
		
		if (User->OwningPlayerController == nullptr)
		{
			User->OwningPlayerController = PC;
			return ID;
		}
	}

	return 255;
}

AGameplayPlayer* UGameplayTeamSubsystem::GetPlayer(uint8 ID) const
{
	if (Players.Contains(ID))
	{
		return Players[ID];
	}
	UE_LOG(LogGameplay, Warning, TEXT("UGameplayPlayer??李얠쓣 ???놁뒿?덈떎. [TeamID: %d]"), ID);
	return nullptr;
}
