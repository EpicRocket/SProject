
#include "MyGameInstance.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
// include Project

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameInstance)

void UMyGameInstance::LoadStartupTable()
{
	K2_OnLoadStartupTable();
    OnLoadStartupTable();
}

void UMyGameInstance::OnLoadStartupTable()
{
}

void UMyGameInstance::LoadTable()
{
    K2_OnTableLoad();
    OnTableLoad();
}

void UMyGameInstance::OnTableLoad()
{
}


/* static */void UMyGameInstanceHelper::LoadStartupTable(UObject* WorldContextObject)
{
	auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	auto GameInstance = Cast<UMyGameInstance>(World->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadStartupTable();
	}
}

/* static */void UMyGameInstanceHelper::LoadTable(UObject* WorldContextObject)
{
	auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	auto GameInstance = Cast<UMyGameInstance>(World->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadTable();
	}
}