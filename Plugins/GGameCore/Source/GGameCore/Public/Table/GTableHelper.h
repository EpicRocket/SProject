// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"

#include "GTableHelper.generated.h"

template<typename T>
concept DerivedFromFTableRowBase = std::is_base_of<FTableRowBase, T>::value;

UCLASS()
class GGAMECORE_API UGTableHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UDataTable* GetTable(uint32 HashKey);

	UFUNCTION(BlueprintCallable, Category = "Table")
	static bool LoadTable();

	UFUNCTION(BlueprintPure, CustomThunk, Category = "Table", meta = (CustomStructureParam = "Data", DisplayName = "GetTableData"))
	static void GetTableData(FTableRowBase& Data, int32 Key);
	DECLARE_FUNCTION(execGetTableData);

	template<DerivedFromFTableRowBase DataT>
	static DataT const* GetTableData(int32 Key)
	{
		UDataTable const* Table = GetTable(GetTypeHash(DataT::StaticStruct()));
		if (!Table)
		{
			return nullptr;
		}

		DataT const* Data = Table->FindRow<DataT>(FName(*FString::FromInt(Key)), FString{});
		if (!Data)
		{
			UE_LOG(LogTemp, Warning, TEXT("Table data not found for Key: %d in table: %s"), Key, *DataT::StaticStruct()->GetName());
			return nullptr;
		}

		return Data;
	}

	template<DerivedFromFTableRowBase DataT>
	static TArray<DataT*> const GetTableDatas()
	{
		UDataTable const* Table = GetTable(GetTypeHash(DataT::StaticStruct()));
		if (!Table)
		{
			return TArray<DataT*>{};
		}

		TArray<DataT*> Datas;
		Table->GetAllRows<DataT>(FString{}, Datas);

		return Datas;
	}
};
