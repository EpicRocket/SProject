// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GErrorManager.generated.h"

struct FGErrorInfo;

DECLARE_LOG_CATEGORY_EXTERN(LogGameError, Log, All);

namespace GameErr
{
	static const FString WORLD_INVALID = TEXT("WORLD_INVALID");
	static const FString OBJECT_INVALID = TEXT("UOBJECT_INVALID");
	static const FString ACTOR_INVALID = TEXT("ACTOR_INVALID");
	static const FString COMPONENT_INVALID = TEXT("COMPONENT_INVALID");
	static const FString WIDGET_INVALID = TEXT("WIDGET_INVALID");
	static const FString SUBSYSTEM_INVALID = TEXT("SUBSYSTEM_INVALID");
	static const FString POINTER_INVALID = TEXT("POINTER_INVALID");
	static const FString VALUE_INVALID = TEXT("VALUE_INVALID");
	static const FString ENUM_INVALID = TEXT("ENUM_INVALID");
	static const FString CLASS_INVALID = TEXT("CLASS_INVALID");
	static const FString TABLE_INVALID = TEXT("TABLE_INVALID");
	static const FString LOAD_FAILURE = TEXT("LOAD_FAILURE");
}

UCLASS()
class GGAMECORE_API UGErrorManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UGErrorManager* Get();

	UFUNCTION(BlueprintCallable)
	void Clear();

	UFUNCTION(BlueprintCallable)
	bool LoadTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable)
	void UnloadTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable)
	FGErrorInfo GetError(FString ErrCode);

private:
	void OverwriteTable(TSharedPtr<struct GErrorTableStack> Stack);

private:
	TMap<FString, TWeakPtr<FGErrorInfo>> Errs;

	TArray<TSharedPtr<struct GErrorTableStack>> ErrsStack;
};

UCLASS()
class GGAMECORE_API UGErrorHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Error", meta = (ReturnDisplayName = "Passed"))
	static bool IsOk(const FGErrorInfo& Err);

	UFUNCTION(BlueprintPure, Category = "Error")
	static const FGErrorInfo& Pass();

	UFUNCTION(BlueprintCallable, Category = "Error")
	static FGErrorInfo Throw(FString ErrCode, FString More);
};

namespace GameCore
{
	GGAMECORE_API bool IsOK(const FGErrorInfo& Err);

	GGAMECORE_API const FGErrorInfo& Pass();

	GGAMECORE_API FGErrorInfo Throw(FString ErrCode, FString More = TEXT(""));
}
