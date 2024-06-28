
#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "Engine/DataTable.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Script.h"

#include "TableSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTable, Log, All);

UCLASS(config = SProject)
class SPROJECT_API UTableSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    static UTableSubsystem* Get();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Table")
    bool LoadTable();

    UDataTable* GetTable(uint32 HashKey) const
    {
        if (!Tables.Contains(HashKey))
        {
            return nullptr;
        }
        return Tables[HashKey];
    }

    template<typename DataT>
    DataT const* GetTableData(int32 Key) const
    {
        UDataTable const* Table = GetTable(GetTypeHash(DataT::StaticStruct()->GetName()));
        if (!Table)
        {
            return nullptr;
        }

        DataT const* Data = Table->FindRow<DataT>(FName(*FString::FromInt(Key)), FString{});

        return Data;
    }

    template<typename DataT>
    TArray<DataT*> const GetTableDatas() const
	{
        UDataTable const* Table = GetTable(GetTypeHash(DataT::StaticStruct()->GetName()));
		if (!Table)
		{
			return TArray<DataT*>{};
		}

		TArray<DataT*> Datas;
		Table->GetAllRows<DataT>(FString{}, Datas);

		return Datas;
	}

    UFUNCTION(BlueprintPure, CustomThunk, Category = "Table", meta = (CustomStructureParam = "Data", DisplayName = "GetTableData"))
    void BP_GetTableData(FTableRowBase& Data, int32 Key);
    DECLARE_FUNCTION(execBP_GetTableData)
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

        UTableSubsystem* TableSubsystem = UTableSubsystem::Get();
        if (!TableSubsystem)
        {
            return;
        }

        UDataTable const* Table = TableSubsystem->GetTable(GetTypeHash(StructType));
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

private:
    UPROPERTY(config)
    FString RootTableRelativePath;

    TMap<uint32, UDataTable*> Tables;
};
