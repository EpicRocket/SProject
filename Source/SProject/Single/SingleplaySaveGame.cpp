﻿
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
	
	/*HACK: 추후 트랙킹 배열을 구현해야 할 수 있음.
			지금은 구현율을 높히기 위해 값이 다르면 전체를 다시 주는 것으로 구현하고
			나중에는 변경 된 값만 보내는 형식으로 수정해줘야함.
	*/
	// 아이템들
	if (SrcUserDocument.Items.Num() != DestDocument.Items.Num())
	{
		Document.Items = SrcUserDocument.Items;
	}
	else
	{
		for (int32 Index = 0; Index < SrcUserDocument.Items.Num(); ++Index)
		{
			if (SrcUserDocument.Items[Index] != DestDocument.Items[Index])
			{
				Document.Items = SrcUserDocument.Items;
				break;
			}
		}
	}

	// 스테이지 정보
	if(SrcUserDocument.Stage.Level != DestDocument.Stage.Level)
	{
		Document.Stage = SrcUserDocument.Stage;
	}


	// 스테이지 상세 정보
	if (SrcUserDocument.StageDetails.Num() != DestDocument.StageDetails.Num())
	{
		Document.StageDetails = SrcUserDocument.StageDetails;
	}
	else
	{
		for (int32 Index = 0; Index < SrcUserDocument.StageDetails.Num(); ++Index)
		{
			if (SrcUserDocument.StageDetails[Index].Level != DestDocument.StageDetails[Index].Level)
			{
				Document.StageDetails = SrcUserDocument.StageDetails;
				break;
			}

			if (SrcUserDocument.StageDetails[Index].Towers.Num() != DestDocument.StageDetails[Index].Towers.Num())
			{
				Document.StageDetails = SrcUserDocument.StageDetails;
				break;
			}
			else
			{
				bool bIsDifferent = false;
				for (int32 TowerIndex = 0; TowerIndex < SrcUserDocument.StageDetails[Index].Towers.Num(); ++TowerIndex)
				{
					if (SrcUserDocument.StageDetails[Index].Towers[TowerIndex] != DestDocument.StageDetails[Index].Towers[TowerIndex])
					{
						Document.StageDetails = SrcUserDocument.StageDetails;
						bIsDifferent = true;
						break;
					}
				}

				if (bIsDifferent)
				{
					break;
				}
			}
		}
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
