
#include "SingleplaySubsystem.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
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

	auto World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}

	auto GameInstance = World->GetGameInstance();
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
		return;
	}

	LoadedSaveGame->SlotName = FString::Printf(TEXT("%s_%s"), *USingleplaySubsystem::SLOT_NAME, *GetGameInstance()->GetName());
}

void USingleplaySubsystem::Load()
{
	if (UGameplayStatics::DoesSaveGameExist(USingleplaySubsystem::SLOT_NAME, 0))
	{
		LoadedSaveGame->LoadSingleplay();
	}
}

USingleplaySaveGameContext* USingleplaySubsystem::GetSaveGame() const
{
	return LoadedSaveGame;
}
