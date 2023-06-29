// Fill out your copyright notice in the Description page of Project Settings.


#include "SFunctionLibrary.h"

void USFunctionLibrary::SortUObjectMapByIntegerKey(TMap<int, UObject*>& Map)
{
	Map.KeySort([](const int& a, const int& b){ return a < b; });
}
