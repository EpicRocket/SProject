// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemInterface.h"

#include "GGameState.generated.h"

class UGAbilitySystemComponent;
enum class EGameplayEffectReplicationMode : uint8;

UCLASS(BlueprintType, Blueprintable, Category = "Core", ClassGroup = "Core")
class GGAMECORE_API AGGameState : public AGameState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	static FName GS_ASC_NAME;

public:
	AGGameState();

protected:
	// AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// ~AActor

public:
	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bIsReplicated_ASC = false;

	UPROPERTY(EditDefaultsOnly)
	EGameplayEffectReplicationMode ReplicationMode_ASC = static_cast<EGameplayEffectReplicationMode>(1U);

private:
	TObjectPtr<UGAbilitySystemComponent> AbilitySystemComponent;

};
