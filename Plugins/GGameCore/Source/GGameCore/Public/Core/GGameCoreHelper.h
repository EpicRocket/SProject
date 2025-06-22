// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Engine.h"

#include "GGameCoreHelper.generated.h"

class APlayerController;
class ULocalPlayer;
class UGGameStateComponent;

UCLASS()
class GGAMECORE_API UGGameCoreBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetPrimaryPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ComponentClass"))
	static UGGameStateComponent* GetGameStateComponent(const UObject* WorldContextObject, TSubclassOf<UGGameStateComponent> ComponentClass);

	template<typename T>
	static T* GetGameStateComponent(const UObject* WorldContextObject)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UGGameStateComponent>::Value, "'T' template parameter to FindComponentByClass must be derived from UGGameStateComponent");
		return (T*)GetGameStateComponent(WorldContextObject, T::StaticClass());
	}

};
