// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Table/GTableHelper.h"
// include GameCore
#include "Table/Subsystem/GTableSubsystem.h"

bool UGTableHelper::LoadTable()
{
	auto Subsystem = UGTableSubsystem::Get();
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->LoadTable();
}

UDataTable* UGTableHelper::GetTable(uint32 HashKey)
{
	auto Subsystem = UGTableSubsystem::Get();
	if (!Subsystem)
	{
		UE_LOG(LogGameTable, Error, TEXT("UGTableSubsystem isn't found"));
		return nullptr;
	}

	if (!Subsystem->Tables.Contains(HashKey))
	{
		UE_LOG(LogGameTable, Error, TEXT("HashKey isn't found"));
		return nullptr;
	}

	return Subsystem->Tables[HashKey];
}


void UGTableHelper::GetTableData(FTableRowBase& Data, int32 Key)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UGTableHelper::execGetTableData)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* OutDataPtr = Stack.MostRecentPropertyAddress;
	if (!OutDataPtr)
	{
		return;
	}
	FTableRowBase& Data = *(FTableRowBase*)OutDataPtr;

	FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!StructProperty)
	{
		return;
	}

	UStruct* StructType = StructProperty->Struct;
	if (!StructType)
	{
		return;
	}

	P_GET_PROPERTY(FIntProperty, Key);
	P_FINISH;

	UGTableSubsystem* TableSubsystem = UGTableSubsystem::Get();
	if (!TableSubsystem)
	{
		return;
	}

	UDataTable const* Table = GetTable(GetTypeHash(StructType));
	if (!Table)
	{
		return;
	}

	uint8* RowMemory = Table->FindRowUnchecked(FName(*FString::FromInt(Key)));
	if (RowMemory)
	{
		FMemory::Memcpy(OutDataPtr, RowMemory, StructType->GetStructureSize());
	}
}
