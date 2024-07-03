
#include "SingleplaySubsystem.h"
// include Engine
#include "Kismet/GameplayStatics.h"
// include Project
#include "SingleplaySaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleplaySubsystem)

DEFINE_LOG_CATEGORY(LogSingleplay)

FString USingleplaySubsystem::SLOT_NAME = TEXT("Singleplay");
int32 USingleplaySubsystem::SLOT_START_INDEX = 0;
int32 USingleplaySubsystem::SLOT_MAX_COUNT = 5;

void USingleplaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void USingleplaySubsystem::LoadSaveGames()
{
	for (int32 Index = USingleplaySubsystem::SLOT_START_INDEX; Index < SLOT_MAX_COUNT; ++Index)
	{
		auto NewContext = NewObject<USingleplaySaveGameContext>(this, USingleplaySaveGameContext::StaticClass());
		if (!IsValid(NewContext))
		{
			UE_LOG(LogSingleplay, Error, TEXT("SaveGameContext 생성에 실패하였습니다."));
			return;
		}
		NewContext->SlotName = FString::Printf(TEXT("%s_%d"), *USingleplaySubsystem::SLOT_NAME, Index);
		Contexts.Emplace(NewContext);

		if (UGameplayStatics::DoesSaveGameExist(NewContext->SlotName, 0))
		{
			if (!NewContext->LoadSingleplay())
			{
				UE_LOG(LogSingleplay, Error, TEXT("SaveGame 로드에 실패하였습니다."))
			}
		}
	}
}

void USingleplaySubsystem::SaveSelectedSaveGame()
{
	if (!IsValid(SelectedSaveGame))
	{
		UE_LOG(LogSingleplay, Warning, TEXT("저장할 SaveGame이 선택되지 않았습니다."))
		return;
	}

	if (!SelectedSaveGame->SaveSingleplay())
	{
		UE_LOG(LogSingleplay, Warning, TEXT("SaveGame 저장에 실패하였습니다."))
			return;
	}
}

TArray<USingleplaySaveGameContext*> USingleplaySubsystem::GetSaveGames() const
{
	return Contexts;
}

void USingleplaySubsystem::DeleteSaveGame(USingleplaySaveGameContext* DeleteSaveGame)
{
	if (!IsValid(DeleteSaveGame))
	{
		UE_LOG(LogSingleplay, Warning, TEXT("삭제할 SaveGame이 선택되지 않았습니다."))
		return;
	}

	DeleteSaveGame->DeleteSignleplay();
}

void USingleplaySubsystem::SelectSaveGame(USingleplaySaveGameContext* SelecteSaveGame)
{
	SelectedSaveGame = SelecteSaveGame;
}
