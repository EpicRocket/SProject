
#include "SingleplaySaveGame.h"
// include Engine
#include "UObject/Package.h"
#include "Kismet/GameplayStatics.h"
// include Project
#include "SingleplaySubsystem.h"
#include "Types/FetchDocument.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleplaySaveGame)


bool USingleplaySaveGameContext::IsExistSaveGame() const
{
	return IsValid(SaveGame) && !SlotName.IsEmpty();
}

void USingleplaySaveGameContext::InitSingleplay()
{
	SaveGame = NewObject<USingleplaySaveGame>(GetTransientPackage(), USingleplaySaveGame::StaticClass());
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSingleplay, Error, TEXT("SaveGame 생성에 실패하였습니다."));
	}
}

bool USingleplaySaveGameContext::LoadSingleplay()
{
	SaveGame = Cast<USingleplaySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!IsValid(SaveGame))
	{
		// NOTE: 잘못된 세이브 게임은 삭제
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
		SaveGame = nullptr;
	}
	return false;
}

bool USingleplaySaveGameContext::SaveSingleplay()
{
	if (IsExistSaveGame())
	{
		return UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
	return false;
}

void USingleplaySaveGameContext::DeleteSignleplay()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}
	SaveGame = nullptr;
}

FFetchDocument USingleplaySaveGameContext::FetchDocument()
{
	FFetchDocument Document;
	if (!IsValid(SaveGame))
	{
		return Document;
	}

	// NOTE: 저장 정보가 추가 될 때 마다 추가
	Document.UserInfo = SaveGame->UserInfo;
	Document.Gold = SaveGame->Gold;
	Document.Cash = SaveGame->Cash;
	Document.Items = SaveGame->Items;
	Document.DomainBuildings = SaveGame->DomainBuildings;

	return Document;
}
