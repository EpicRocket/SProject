
#include "SuperGameState.h"
// include Engine
#include "Engine/World.h"
#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
// include Project
#include "AbilitySystem/SuperAbilitySystemComponent.h"
#include "Character/Unit.h"
#include "GameplayMessage/GameplayMessage_UnitEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SuperGameState)

UE_DEFINE_GAMEPLAY_TAG(Unit_Event_Death, "Unit.Event.Death")
UE_DEFINE_GAMEPLAY_TAG(Unit_Event_Spawned, "Unit.Event.Spawned")

ASuperGameState::ASuperGameState()
{
	AbilitySystemComponent = CreateDefaultSubobject<USuperAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* ASuperGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

AUnit* ASuperGameState::SpawnUnit(const FVector& Location, const FRotator& Rotation, TSubclassOf<AUnit> UnitClass)
{
	if (!UnitClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.CustomPreSpawnInitalization = [this](AActor* SpawnedActor)
	{
		if (AUnit* Unit = Cast<AUnit>(SpawnedActor))
		{
			Unit->UnitInitailize();
		}
	};

	AUnit* Unit = World->SpawnActor<AUnit>(UnitClass.Get(), Location, Rotation, SpawnParams);
	if (!IsValidUnit(Unit))
	{
		if (IsValid(Unit))
		{
			Unit->Destroy();
		}
		return nullptr;
	}

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(this);
	TWeakObjectPtr<ASuperGameState> WeakObject(this);

	FGameplayMessageListenerParams<FGameplayMessage_UnitEvent> ListenerParams;
	ListenerParams.OnMessageReceivedCallback = [this, WeakObject](FGameplayTag GameplayTag, const FGameplayMessage_UnitEvent& GameplayMessage)
	{
		if (ASuperGameState* StrongObject = WeakObject.Get())
		{
			if (IsValid(GameplayMessage.Destination))
			{
				CacheUnitMap.Remove(GameplayMessage.Destination->UnitId);
			}
		}
	};
	GameplayMessageSubsystem.RegisterListener(Unit_Event_Death, ListenerParams);

	CacheUnitMap.Emplace(Unit->UnitId, Unit);

	GameplayMessageSubsystem.BroadcastMessage(Unit_Event_Spawned, FGameplayMessage_UnitEvent{ .Destination = Unit });

	return Unit;
}
