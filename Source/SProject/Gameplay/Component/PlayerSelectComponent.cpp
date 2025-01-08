
#include "PlayerSelectComponent.h"
// include GameCore
#include "Core/GPlayerController.h"
// include Project
#include "Gameplay/Interface/ISelectableActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerSelectComponent)

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

	Actor->Execute_SelectActor(ActorObject, GetOwningPlayer<AGPlayerController>());
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

	Actor->Execute_DeselectActor(ActorObject, GetOwningPlayer<AGPlayerController>());
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
		Actor->Execute_DeselectActor(ActorObject, GetOwningPlayer<AGPlayerController>());
	}
	SelectedActors.Empty();
}

bool UPlayerSelectComponent::IsSelected(TScriptInterface<ISelectableActor> Actor) const
{
	return SelectedActors.Contains(Actor);
}
