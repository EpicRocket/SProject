
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemInterface.h"

#include "GGameState.generated.h"

class UGAbilitySystemComponent;
enum class EGameplayEffectReplicationMode : uint8;

UCLASS(BlueprintType, Blueprintable)
class GGAMECORE_API AGGameState : public AGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

	static FName GS_ASC_NAME;

public:
	AGGameState();

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

protected:
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 시스템")
	bool bIsReplicated_ASC = false;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 시스템")
	EGameplayEffectReplicationMode ReplicationMode_ASC = static_cast<EGameplayEffectReplicationMode>(1U);

private:
	TObjectPtr<UGAbilitySystemComponent> AbilitySystemComponent;
};
