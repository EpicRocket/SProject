// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SProjectBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SPROJECT_API USProjectBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Sort")
	static void SortUObjectMapByIntegerKey(UPARAM(ref) TMap<int, UObject*>& Map);
	
};
