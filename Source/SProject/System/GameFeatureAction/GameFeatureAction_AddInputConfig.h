// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddInputConfig.generated.h"

class APawn;
struct FMappableConfigPair;
struct FComponentRequestHandle;

/**
 * Registers a Player Mappable Input config to the Game User Settings
 * 
 * Expects that local players are set up to use the EnhancedInput system.
 */
UCLASS(meta = (DisplayName = "Add Input Config"))
class UGameFeatureAction_AddInputConfig : public UGameFeatureAction
{
	GENERATED_BODY()
	
public:
	//~UObject UGameFeatureAction
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureUnregistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~End of UGameFeatureAction interface

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif

private:	
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};
	
	/** The "active data" that is used with this game feature's context changes. */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext);
	void Reset(FPerContextData& ActiveData);
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	/** The player mappable configs to register for user with this config */
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> InputConfigs;
};
