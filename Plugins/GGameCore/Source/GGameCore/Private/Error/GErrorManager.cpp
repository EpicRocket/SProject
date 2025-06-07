
#include "Error/GErrorManager.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
// include GameCore
#include "Error/GErrorTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GErrorManager)

DEFINE_LOG_CATEGORY(LogGameError)

struct GErrorTableStack
{
	FString DataTableName;
	TMap<FString, TSharedPtr<FGErrorInfo>> Errs;
};

/* static */UGErrorManager* UGErrorManager::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UGErrorManager>();
}

void UGErrorManager::Clear()
{
	Errs.Empty();
	ErrsStack.Empty();
}

bool UGErrorManager::LoadTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogGameError, Warning, TEXT("로드 할 데이터 테이블을 찾지 못하였습니다."));
		return false;
	}

	auto DataTableName = DataTable->GetName();
	auto& RowMap = DataTable->GetRowMap();

	TSharedPtr<GErrorTableStack> Stack = nullptr;

	for (auto Iter = ErrsStack.CreateIterator(); Iter; ++Iter)
	{
		if ((*Iter)->DataTableName == DataTableName)
		{
			Stack = *Iter;
			break;
		}
	}

	if (Stack == nullptr)
	{
		Stack = MakeShared<GErrorTableStack>();
		Stack->DataTableName = DataTableName;
		Stack->Errs.Reserve(RowMap.Num());

		for (auto& [Key, Value] : RowMap)
		{
			auto RowName = Key.ToString();
			auto ErrValue = reinterpret_cast<FGErrorInfo*>(Value);
			Stack->Errs.Emplace(RowName, MakeShared<FGErrorInfo>(*ErrValue));
		}
	}

	OverwriteTable(Stack);
	ErrsStack.Add(Stack);

	return true;
}

void UGErrorManager::UnloadTable(UDataTable* DataTable)
{
	if (!IsValid(DataTable))
	{
		UE_LOG(LogGameError, Warning, TEXT("언로드 할 데이터 테이블을 찾지 못하였습니다."));
		return;
	}

	auto DataTableName = DataTable->GetName();
	int32 FindIndex = INDEX_NONE;

	for (int32 Index = 0; Index < ErrsStack.Num(); ++Index)
	{
		if (ErrsStack[Index]->DataTableName == DataTableName)
		{
			FindIndex = Index;
			break;
		}
	}

	if (FindIndex != INDEX_NONE)
	{
		bool bLastIndex = FindIndex == ErrsStack.Num() - 1;
		ErrsStack.RemoveAt(FindIndex);
		if (bLastIndex)
		{
			if (ErrsStack.IsEmpty())
			{
				ErrsStack.Empty();
			}
			else
			{
				OverwriteTable(ErrsStack.Last());
			}
		}
	}
	else
	{
		UE_LOG(LogGameError, Warning, TEXT("%s 데이터 테이블 언로드 실패하였습니다."), *DataTableName);
	}
}

FGErrorInfo UGErrorManager::GetError(FString ErrCode)
{
	if (Errs.Contains(ErrCode))
	{
		auto& Err = Errs[ErrCode];
		if (Err.IsValid())
		{
			return *Err.Pin();
		}
	}

	FGErrorInfo Err;
	Err.ErrType = EGErrType::Error;
	Err.ErrCode = ErrCode;
	Err.Description = FText::FromString(TEXT("Unkown Error"));

	return Err;
}

void UGErrorManager::OverwriteTable(TSharedPtr<struct GErrorTableStack> Stack)
{
	for (auto& [RowName, ErrValue] : Stack->Errs)
	{
		if (Errs.Contains(RowName))
		{
			UE_LOG(LogGameError, Verbose, TEXT("%s키 값이 덮어씌워졌습니다."), *RowName);
			Errs[RowName] = ErrValue;
		}
		else
		{
			Errs.Emplace(RowName, ErrValue);
		}
	}
}

bool UGErrorHelper::IsOk(const FGErrorInfo& Err)
{
	return GameCore::IsOK(Err);
}

const FGErrorInfo& UGErrorHelper::Pass()
{
	return GameCore::Pass();
}

FGErrorInfo UGErrorHelper::Throw(FString ErrCode, FString More)
{
	return GameCore::Throw(ErrCode, More);
}

bool GameCore::IsOK(const FGErrorInfo& Err)
{
	switch (Err.ErrType)
	{
	case EGErrType::None:
		return true;

	case EGErrType::Verbose:
		return true;

	case EGErrType::Warning:
		return true;

	case EGErrType::Error:
		return false;
	}

	return false;
}

const FGErrorInfo& GameCore::Pass()
{
	static FGErrorInfo NotErr;
	return NotErr;
}

GGAMECORE_API FGErrorInfo GameCore::Throw(FString ErrCode, FString More)
{
	auto Manager = UGErrorManager::Get();
	check(Manager);

	auto Err = Manager->GetError(ErrCode);

	switch (Err.ErrType)
	{
	case EGErrType::Verbose:
		UE_LOG(LogGameError, Error, TEXT("%s(%s)\n%s"), *Err.Description.ToString(), *Err.ErrCode, *More);
		break;

	case EGErrType::Warning:
		UE_LOG(LogGameError, Error, TEXT("%s(%s)\n%s"), *Err.Description.ToString(), *Err.ErrCode, *More);
		break;

	case EGErrType::Error:
		UE_LOG(LogGameError, Error, TEXT("%s(%s)\n%s"), *Err.Description.ToString(), *Err.ErrCode, *More);
		break;
	}

	return Err;
}
