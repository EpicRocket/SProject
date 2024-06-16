
#include "UI/GUIManagerSubsystem.h"

#include "Core/GLocalPlayer.h"
#include "UI/GUIPolicy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GUIManagerSubsystem)

void UGUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GUIPolicyClassPtr.IsNull())
	{
		TSubclassOf<UGUIPolicy> GUIPolicyClass = GUIPolicyClassPtr.LoadSynchronous();
		GUIPolicy = NewObject<UGUIPolicy>(this, GUIPolicyClass);
	}
}

void UGUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	GUIPolicy = nullptr;
}

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
	if (IsValid(GUIPolicy))
	{
		GUIPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UGUIManagerSubsystem::NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer)
{
	if (IsValid(GUIPolicy))
	{
		GUIPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UGUIManagerSubsystem::NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer)
{
	if (IsValid(GUIPolicy))
	{
		GUIPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}

UGUIPolicy* UGUIManagerSubsystem::GetPolicy() const
{
	return GUIPolicy;
}
