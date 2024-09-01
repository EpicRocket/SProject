
#include "SingleplaySubsystem.h"
// include Engine
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Package.h"
// include Project
#include "SingleplaySaveGame.h"
#include "Core/GLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SingleplaySubsystem)

DEFINE_LOG_CATEGORY(LogSingleplay)

FString USingleplaySubsystem::SLOT_NAME = TEXT("Singleplay");

USingleplaySubsystem* USingleplaySubsystem::Get(const ULocalPlayer* LocalPlayer)
{
	if (!IsValid(LocalPlayer))
	{
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<USingleplaySubsystem>();
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

	LoadedSaveGame->SlotName = FString::Printf(TEXT("%s_temp"), *USingleplaySubsystem::SLOT_NAME);
}

void USingleplaySubsystem::Load()
{
	auto GPlayer = Cast<UGLocalPlayer>(GetLocalPlayer());
	if (!IsValid(GPlayer))
	{
		return;
	}

	LoadedSaveGame->SlotName = FString::Printf(TEXT("%s_%d"), *USingleplaySubsystem::SLOT_NAME, GPlayer->UniqueId);

	if (UGameplayStatics::DoesSaveGameExist(USingleplaySubsystem::SLOT_NAME, 0))
	{
		LoadedSaveGame->LoadSingleplay();
	}
}

USingleplaySaveGameContext* USingleplaySubsystem::GetSaveGame() const
{
	return LoadedSaveGame;
}
