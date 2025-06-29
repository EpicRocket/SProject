// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayHelper.generated.h"

class AGameplayPlayer;
class UGameplayPlayerComponent;
class UActorComponent;
struct FGErrorInfo;

UCLASS()
class GGAMECORE_API UGameplayHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "GGameCore", meta = (WorldContext = "WorldContextObject"))
    static AGameplayPlayer* GetGameplayPlayer(const UObject* WorldContextObject, uint8 TeamID);

	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ComponentClass", DynamicOutputParam = "FindComponent", ReturnDisplayName = "Error"))
	static FGErrorInfo GetGameplayPlayerComponentByPC(class APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo SetGameplayTagByInt32(AActor* Actor, FName TagName, int32 TagValue);

	UFUNCTION(BlueprintPure)
	static FGErrorInfo GetGameplayTagByInt32(AActor* Actor, FName TagName, int32& TagValue);

	UFUNCTION(BlueprintPure)
	static bool IsFarFrom(const FVector& A, const FVector& B, float SourceRadius = 1.0F);

};
