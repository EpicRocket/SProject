
#include "UI/GUIManagerSubsystem.h"

#include "Core/GLocalPlayer.h"
#include "UI/GUIPolicy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GUIManagerSubsystem)

bool UGUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		return ChildClasses.Num() == 0;
	}
	return false;
}

void UGUIManagerSubsystem::NotifyPlayerAdded(UGLocalPlayer* LocalPlayer)
{
	if (IsValid(Policy))
	{
		Policy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UGUIManagerSubsystem::NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer)
{
	if (IsValid(Policy))
	{
		Policy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UGUIManagerSubsystem::NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer)
{
	if (IsValid(Policy))
	{
		Policy->NotifyPlayerDestroyed(LocalPlayer);
	}
}

UGUIPolicy* UGUIManagerSubsystem::GetPolicy() const
{
	return Policy;
}
