// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Script.h"
#include "NativeGameplayTags.h"
#include <type_traits>

#include "GTableSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameTable, Log, All);

DECLARE_MULTICAST_DELEGATE(FTableLoadCompleted);

UCLASS(config = SProject)
class GGAMECORE_API UGTableSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UGTableSubsystem* Get();

	FTableLoadCompleted TableLoadCompleted;

	bool LoadTable();

public:
	UPROPERTY()
	TMap<uint32, UDataTable*> Tables;

};
