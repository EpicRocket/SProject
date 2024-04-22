#include "MyUISubsystem.h"

void UMyUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (!Policy && !PolicyClass.IsNull())
    {
        
    }
}

void UMyUISubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool UMyUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return false;
}

void UMyUISubsystem::NotifyPlayerAdded(UMyLocalPlayer* LocalPlayer)
{
}

void UMyUISubsystem::NotifyPlayerRemoved(UMyLocalPlayer* LocalPlayer)
{
}

void UMyUISubsystem::NotifyPlayerDestroyed(UMyLocalPlayer* LocalPlayer)
{
}
