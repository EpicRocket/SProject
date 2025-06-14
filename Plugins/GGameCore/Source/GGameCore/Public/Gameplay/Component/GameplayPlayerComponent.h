
#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayPlayerComponent.generated.h"

class AGameplayPlayer;
struct FLatentActionInfo;
struct FGErrorInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume), Category = "Gameplay", ClassGroup = "Gameplay")
class GGAMECORE_API UGameplayPlayerComponent : public UGGameCoreComponent
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
class GGAMECORE_API UGameplayPlayerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ComponentClass", DynamicOutputParam = "FindComponent", ReturnDisplayName = "Error"))
	static FGErrorInfo GetGameplayPlayerComponentByPC(class APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent);

};
