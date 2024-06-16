
#include "MyUISubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

UMyUISubsystem* UMyUISubsystem::Get(const UObject* WorldContextObject)
{
    if (!ensure(WorldContextObject))
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (World == nullptr)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (GameInstance == nullptr)
	{
		return nullptr;
	}

    return GameInstance->GetSubsystem<UMyUISubsystem>();
}
