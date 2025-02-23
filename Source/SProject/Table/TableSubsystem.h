
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

#include "TableSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTable, Log, All);

template<typename T>
concept DerivedFromFTableRowBase = std::is_base_of<FTableRowBase, T>::value;

DECLARE_MULTICAST_DELEGATE(FTableLoadCompleted);

/*
 * 테이터 테이블 시스템
 */
UCLASS(config = SProject)
class MY_API UTableSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    static UTableSubsystem* Get();

	FTableLoadCompleted TableLoadCompleted;

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

    template<DerivedFromFTableRowBase DataT>
    DataT const* GetTableData(int32 Key) const
    {
        UDataTable const* Table = GetTable(GetTypeHash(DataT::StaticStruct()));
        if (!Table)
        {
            return nullptr;
        }

        DataT const* Data = Table->FindRow<DataT>(FName(*FString::FromInt(Key)), FString{});

        return Data;
    }

    template<DerivedFromFTableRowBase DataT>
    TArray<DataT*> const GetTableDatas() const
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

    UFUNCTION(BlueprintPure, CustomThunk, Category = "Table", meta = (CustomStructureParam = "Data", DisplayName = "GetTableData"))
    void K2_GetTableData(FTableRowBase& Data, int32 Key);
    DECLARE_FUNCTION(execK2_GetTableData)
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

UCLASS()
class MY_API UTableHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    template<typename T>
    static T const* GetData(int32 Key)
    {
        auto Subsystem = UTableSubsystem::Get();
        if (Subsystem == nullptr)
        {
            return nullptr;
        }
		return Subsystem->GetTableData<T>(Key);
    }

	template<typename T>
	static TArray<T*> const GetDatas()
	{
		auto Subsystem = UTableSubsystem::Get();
		if (Subsystem == nullptr)
		{
			return TArray<T*>{};
		}
		return Subsystem->GetTableDatas<T>();
	}
};
