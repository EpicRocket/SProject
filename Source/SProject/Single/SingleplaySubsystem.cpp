
#include "SingleplaySubsystem.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Package.h"
// include Project
#include "SingleplaySaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleplaySubsystem)

DEFINE_LOG_CATEGORY(LogSingleplay)

FString USingleplaySubsystem::SLOT_NAME = TEXT("Singleplay");

USingleplaySubsystem* USingleplaySubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<USingleplaySubsystem>();
}

void USingleplaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadedSaveGame = NewObject<USingleplaySaveGameContext>(this, USingleplaySaveGameContext::StaticClass());
	if (!IsValid(LoadedSaveGame))
	{
		UE_LOG(LogSingleplay, Error, TEXT("SaveGameContext 생성에 실패하였습니다."));
		return;
	}

	LoadedSaveGame->SlotName = USingleplaySubsystem::SLOT_NAME;

	if (UGameplayStatics::DoesSaveGameExist(USingleplaySubsystem::SLOT_NAME, 0))
	{
		LoadedSaveGame->LoadSingleplay();
	}
}

bool USingleplaySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if ALLOW_SINGLEPLAY
	return true;
#else
	return false;
#endif
}

USingleplaySaveGameContext* USingleplaySubsystem::GetSaveGame() const
{
	return LoadedSaveGame;
}
