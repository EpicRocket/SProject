
#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayPlayerComponent.generated.h"

class AGameplayPlayer;
struct FLatentActionInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UGameplayPlayerComponent : public UGGameCoreComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AGameplayPlayer* GetGameplayPlayer() const;
	
	template <class T>
	T* GetGameplayPlayer() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AGameplayPlayer>::Value, "T는 AGameplayPlayer 타입이 아닙니다.");
		return Cast<T>(GetOwner());
	}

};

UCLASS()
class MY_API UGameplayPlayerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ComponentClass", DynamicOutputParam = "FindComponent"))
	static void GetGameplayPlayerComponentByPC(class APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent);

};
