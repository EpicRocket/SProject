// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GGameCoreHelper.generated.h"

class APlayerController;
class ULocalPlayer;
class UGGameModeComponent;
class UGGameStateComponent;
class UGPlayerComponent;

UCLASS()
class GGAMECORE_API UGGameCoreHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject"))
	static ULocalPlayer* GetPrimaryLocalPlayer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetPrimaryPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ComponentClass"))
	static UGGameModeComponent* GetGameModeComponent(const UObject* WorldContextObject, TSubclassOf<UGGameModeComponent> ComponentClass);

	template<typename T>
	static T* GetGameModeComponent(const UObject* WorldContextObject)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UGGameModeComponent>::Value, "'T' template parameter to FindComponentByClass must be derived from UGGameModeComponent");
		return (T*)GetGameModeComponent(WorldContextObject, T::StaticClass());
	}

	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ComponentClass"))
	static UGGameStateComponent* GetGameStateComponent(const UObject* WorldContextObject, TSubclassOf<UGGameStateComponent> ComponentClass);

	template<typename T>
	static T* GetGameStateComponent(const UObject* WorldContextObject)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UGGameStateComponent>::Value, "'T' template parameter to FindComponentByClass must be derived from UGGameStateComponent");
		return (T*)GetGameStateComponent(WorldContextObject, T::StaticClass());
	}

	UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (DeterminesOutputType = "ComponentClass"))
	static UGPlayerComponent* GetPlayerControllerComponent(const APlayerController* PlayerController, TSubclassOf<UGPlayerComponent> ComponentClass);

	template<typename T>
	static T* GetPlayerControllerComponent(const APlayerController* PlayerController)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UGPlayerComponent>::Value, "'T' template parameter to FindComponentByClass must be derived from UGPlayerComponent");
		return (T*)GetPlayerControllerComponent(PlayerController, T::StaticClass());
	}

};
