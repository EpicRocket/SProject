
#include "PlayerSelectComponent.h"
// include GameCore
#include "GameFramework/PlayerController.h"
// include Project
#include "Gameplay/Interface/ISelectableActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerSelectComponent)

//////////////////////////////////////////////////////////////////////////
// UPlayerSelectComponent
//////////////////////////////////////////////////////////////////////////

void UPlayerSelectComponent::Select(TScriptInterface<ISelectableActor> Actor)
{
	auto ActorObject = Actor.GetObject();
	if (!ActorObject)
	{
		return;
	}

	if (!Actor->Execute_IsSelectableActor(ActorObject))
	{
		return;
	}

	if (!SelectedActors.Contains(Actor))
	{
		SelectedActors.Emplace(Actor);
	}

	Actor->Execute_SelectActor(ActorObject, GetOwningPlayer());
}

void UPlayerSelectComponent::Deselect(TScriptInterface<ISelectableActor> Actor)
{
	auto ActorObject = Actor.GetObject();
	if (!ActorObject)
	{
		return;
	}

	if (!SelectedActors.Contains(Actor))
	{
		return;
	}

	Actor->Execute_DeselectActor(ActorObject, GetOwningPlayer());
	SelectedActors.Remove(Actor);
}

void UPlayerSelectComponent::ClearSelection()
{
	for (auto& Actor : SelectedActors)
	{
		auto ActorObject = Actor.GetObject();
		if (!ActorObject)
		{
			continue;
		}
		Actor->Execute_DeselectActor(ActorObject, GetOwningPlayer());
	}
	SelectedActors.Empty();
}

void UPlayerSelectComponent::Exclude(const TArray<TScriptInterface<ISelectableActor>>& Actors)
{
	for (auto& Actor : SelectedActors)
	{
		if (!Actors.Contains(Actor))
		{
			auto ActorObject = Actor.GetObject();
			if (!ActorObject)
			{
				continue;
			}
			Actor->Execute_DeselectActor(ActorObject, GetOwningPlayer());
		}
	}
	SelectedActors = Actors;
}

bool UPlayerSelectComponent::IsSelected(TScriptInterface<ISelectableActor> Actor) const
{
	return SelectedActors.Contains(Actor);
}

//////////////////////////////////////////////////////////////////////////
// UPlayerSelectFunctionLibrary
//////////////////////////////////////////////////////////////////////////

void UPlayerSelectFunctionLibrary::SelectActor(APlayerController* PC, AActor* Target)
{
	if (!IsValid(PC))
	{
		return;
	}

	if (!IsValid(Target))
	{
		return;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	if (Target->Implements<USelectableActor>())
	{
		SelectableActorInterface.SetObject(Target);
	}
	else
	{
		ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
		if (!SelectableActor)
		{
			return;
		}

		SelectableActorInterface.SetObject(Target);
		SelectableActorInterface.SetInterface(SelectableActor);
	}

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return;
	}

	PlayerSelectComponent->Select(SelectableActorInterface);
}

void UPlayerSelectFunctionLibrary::DeselectActor(APlayerController* PC, AActor* Target)
{
	if (!IsValid(PC))
	{
		return;
	}

	if (!IsValid(Target))
	{
		return;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	if (Target->Implements<USelectableActor>())
	{
		SelectableActorInterface.SetObject(Target);
	}
	else
	{
		ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
		if (!SelectableActor)
		{
			return;
		}

		SelectableActorInterface.SetObject(Target);
		SelectableActorInterface.SetInterface(SelectableActor);
	}

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return;
	}

	PlayerSelectComponent->Deselect(SelectableActorInterface);
}

void UPlayerSelectFunctionLibrary::ClearSelection(APlayerController* PC)
{
	if (!IsValid(PC))
	{
		return;
	}

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return;
	}

	PlayerSelectComponent->ClearSelection();
}

void UPlayerSelectFunctionLibrary::ClearSelectionExceptOther(APlayerController* PC, const TSet<AActor*>& Other)
{
	if (!IsValid(PC))
	{
		return;
	}

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return;
	}

	TArray<TScriptInterface<ISelectableActor>> OtherSelectableActors;
	for (auto& Target : Other)
	{
		TScriptInterface<ISelectableActor> SelectableActorInterface;
		if (Target->Implements<USelectableActor>())
		{
			SelectableActorInterface.SetObject(Target);
			OtherSelectableActors.Emplace(SelectableActorInterface);
		}
		else
		{
			ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
			if (SelectableActor)
			{
				SelectableActorInterface.SetObject(Target);
				SelectableActorInterface.SetInterface(SelectableActor);
				OtherSelectableActors.Emplace(SelectableActorInterface);
			}
		}
	}

	PlayerSelectComponent->Exclude(OtherSelectableActors);
}

bool UPlayerSelectFunctionLibrary::IsSelected(APlayerController* PC, AActor* Target)
{
	if (!IsValid(PC))
	{
		return false;
	}

	if (!IsValid(Target))
	{
		return false;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	if (Target->Implements<USelectableActor>())
	{
		SelectableActorInterface.SetObject(Target);
	}
	else
	{
		ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
		if (!SelectableActor)
		{
			return false;
		}

		SelectableActorInterface.SetObject(Target);
		SelectableActorInterface.SetInterface(SelectableActor);
	}

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return false;
	}

	return PlayerSelectComponent->IsSelected(SelectableActorInterface);
}
