// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperFunctionLibrary.h"

void USuperFunctionLibrary::SortUObjectMapByIntegerKey(TMap<int, UObject*>& Map)
{
	Map.KeySort([](const int& a, const int& b){ return a < b; });
}
