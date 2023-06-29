// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatureAction_AddInputConfig.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "System/SMappableConfigPair.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputConfig)

#define LOCTEXT_NAMESPACE "GameFeatures_AddInputConfig"

void UGameFeatureAction_AddInputConfig::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	// Register the input configs with the local settings, this way the data inside them is available all the time
	// and not just when this game feature is active. This is necessary for displaying key binding options
	// on the main menu, or other times when the game feature may not be active.
	for (const FMappableConfigPair& Pair : InputConfigs)
	{
		FMappableConfigPair::RegisterPair(Pair);
	}
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	for (const FMappableConfigPair& Pair : InputConfigs)
	{
		FMappableConfigPair::UnregisterPair(Pair);
	}
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	// if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
	// 	!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	// {
	// 	Reset(ActiveData);
	// }
	
	for (const FMappableConfigPair& Pair : InputConfigs)
	{
		FMappableConfigPair::ActivatePair(Pair);
	}

	// Call super after the above logic so that we have our context before being added to the world
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
	// FPerContextData* ActiveData = ContextData.Find(Context);
	//
	// if (ensure(ActiveData))
	// {
	// 	Reset(*ActiveData);
	// }
	
	for (const FMappableConfigPair& Pair : InputConfigs)
	{
		FMappableConfigPair::DeactivatePair(Pair);
	}
}