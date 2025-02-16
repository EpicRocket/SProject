
#include "Error/GErrorManager.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
// include GameCore
#include "Error/GErrorTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GErrorManager)

/* static */UGErrorManager* UGErrorManager::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UGErrorManager>();
}

bool UGErrorManager::LoadTable(UDataTable* DataTable)
{
	return false;
}

void UGErrorManager::UnloadTable(UDataTable* DataTable)
{
}

bool UGErrorHelper::IsOk(const FGErrorInfo& Err)
{
	return GameCore::IsOK(Err);
}

FGErrorInfo UGErrorHelper::Pass()
{
	return GameCore::Pass();
}

FGErrorInfo UGErrorHelper::Throw(FString ErrCode, FString More)
{
	return FGErrorInfo();
}

bool GameCore::IsOK(const FGErrorInfo& Err)
{
	switch (Err.ErrType)
	{
	case EGErrType::None:
		return true;

	case EGErrType::Verbose:
		UE_LOG(LogTemp, Verbose, TEXT("%s(%s)"), *Err.Description.ToString(), *Err.ErrCode)
			return true;

	case EGErrType::Warning:
		UE_LOG(LogTemp, Warning, TEXT("%s(%s)"), *Err.Description.ToString(), *Err.ErrCode)
			return true;

	case EGErrType::Error:
		UE_LOG(LogTemp, Error, TEXT("%s(%s)"), *Err.Description.ToString(), *Err.ErrCode)
			return false;
	}

	return false;
}

FGErrorInfo GameCore::Pass()
{
	return FGErrorInfo();
}

GGAMECORE_API FGErrorInfo GameCore::Throw(FString ErrCode, FString More)
{
	return FGErrorInfo();
}
