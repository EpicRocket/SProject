
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

	if (SelectedActors.Contains(Actor))
	{
		return;
	}

	if (!Actor->Execute_IsSelectableActor(ActorObject))
	{
		return;
	}

	Actor->Execute_SelectActor(ActorObject, GetOwningPlayer());
	SelectedActors.Emplace(Actor);
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

	ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
	if (!SelectableActor)
	{
		return;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	SelectableActorInterface.SetObject(Target);
	SelectableActorInterface.SetInterface(SelectableActor);

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

	ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
	if (!SelectableActor)
	{
		return;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	SelectableActorInterface.SetObject(Target);
	SelectableActorInterface.SetInterface(SelectableActor);

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

	ISelectableActor* SelectableActor = Cast<ISelectableActor>(Target);
	if (!SelectableActor)
	{
		return false;
	}

	TScriptInterface<ISelectableActor> SelectableActorInterface;
	SelectableActorInterface.SetObject(Target);
	SelectableActorInterface.SetInterface(SelectableActor);

	auto PlayerSelectComponent = PC->GetComponentByClass<UPlayerSelectComponent>();
	if (!IsValid(PlayerSelectComponent))
	{
		return false;
	}

	return PlayerSelectComponent->IsSelected(SelectableActorInterface);
}
