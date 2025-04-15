
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
		return;
	}

	SaveGame = NewObject<USingleplaySaveGame>(GetTransientPackage(), USingleplaySaveGame::StaticClass());
	if (!IsValid(SaveGame))
	{
		return;
	}

	SaveGame->UserDocument = SingleplaySettings->UserDocument;
}

bool USingleplaySaveGameContext::LoadSingleplay()
{
	SaveGame = Cast<USingleplaySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!IsValid(SaveGame))
	{
		// NOTE: ?섎せ???몄씠釉?寃뚯엫? ??젣
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

	if (SrcUserDocument.UserInfo != DestDocument.UserInfo)
	{
		Document.UserInfo = SrcUserDocument.UserInfo;
	}

	if (SrcUserDocument.Gold != DestDocument.Gold)
	{
		Document.Gold = SrcUserDocument.Gold;
	}

	if (SrcUserDocument.Cash != DestDocument.Cash)
	{
		Document.Cash = SrcUserDocument.Cash;
	}
	
	if (SrcUserDocument.Items != DestDocument.Items)
	{
		Document.Items = SrcUserDocument.Items;
	}

	if(SrcUserDocument.LastStageLevel != DestDocument.LastStageLevel)
	{
		Document.LastStageLevel = SrcUserDocument.LastStageLevel;
	}

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
