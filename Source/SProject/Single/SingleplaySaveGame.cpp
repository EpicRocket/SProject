
#include "SingleplaySaveGame.h"
// include Engine
#include "UObject/Package.h"
#include "Kismet/GameplayStatics.h"
// include Project
#include "SingleplaySubsystem.h"
#include "Types/FetchDocumentTypes.h"
#include "SingleplaySettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleplaySaveGame)

FString USingleplaySaveGameContext::Token = TEXT("SINGLE_PLAY");

bool USingleplaySaveGameContext::IsExistSaveGame() const
{
	return IsValid(SaveGame) && !SlotName.IsEmpty();
}

void USingleplaySaveGameContext::InitSingleplay()
{
	auto SingleplaySettings = GetDefault<USingleplaySettings>();
	if (!IsValid(SingleplaySettings))
	{
		UE_LOG(LogSingleplay, Error, TEXT("Singleplay Settings를 불러오지 못했습니다."));
		return;
	}

	SaveGame = NewObject<USingleplaySaveGame>(GetTransientPackage(), USingleplaySaveGame::StaticClass());
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSingleplay, Error, TEXT("SaveGame 생성에 실패하였습니다."));
		return;
	}

	SaveGame->UserDocument = SingleplaySettings->UserDocument;
}

bool USingleplaySaveGameContext::LoadSingleplay()
{
	SaveGame = Cast<USingleplaySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!IsValid(SaveGame))
	{
		// NOTE: 잘못된 세이브 게임은 삭제
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
		SaveGame = nullptr;
		return false;
	}
	return true;
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

	auto const& SrcUserDocument = SaveGame->UserDocument;
	auto const& DestDocument = GetUserDocument().Get();

	// 유저 정보
	if (SrcUserDocument.UserInfo != DestDocument.UserInfo)
	{
		Document.UserInfo = SrcUserDocument.UserInfo;
	}

	// 골드
	if (SrcUserDocument.Gold != DestDocument.Gold)
	{
		Document.Gold = SrcUserDocument.Gold;
	}

	// 캐시
	if (SrcUserDocument.Cash != DestDocument.Cash)
	{
		Document.Cash = SrcUserDocument.Cash;
	}
	
	// 아이템들
	if (SrcUserDocument.Items != DestDocument.Items)
	{
		Document.Items = SrcUserDocument.Items;
	}

	// 스테이지 정보
	if(SrcUserDocument.LastStageLevel != DestDocument.LastStageLevel)
	{
		Document.LastStageLevel = SrcUserDocument.LastStageLevel;
	}

	// 스테이지 상세 정보
	if (SrcUserDocument.Stages != DestDocument.Stages)
	{
		Document.Stages = SrcUserDocument.Stages;
	}

	OnUpdateUserDocument();

	return Document;
}

TSharedRef<FUserDocument> USingleplaySaveGameContext::GetUserDocument() const
{
	if (!UserDocumentPtr.IsValid())
	{
		UserDocumentPtr = MakeShared<FUserDocument>();
	}
	return UserDocumentPtr.ToSharedRef();
}

void USingleplaySaveGameContext::OnUpdateUserDocument()
{
	if (!IsValid(SaveGame))
	{
		return;
	}

	GetUserDocument().Get() = SaveGame->UserDocument;
}
