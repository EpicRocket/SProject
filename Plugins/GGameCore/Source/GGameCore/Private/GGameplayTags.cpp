// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "GGameplayTags.h"

namespace GGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "Event that fires on death. This event only fires on the server.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Movement_Slowed, "Status.Movement.Slowed", "Target is slowed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Movement_Rooted, "Status.Movement.Rooted", "Target is rooted");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Movement_Stunned, "Status.Movement.Stunned", "Target is stunned");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Movement_Banned, "Status.Movement.Banned", "Target's move is banned. Permanent effect.");
}