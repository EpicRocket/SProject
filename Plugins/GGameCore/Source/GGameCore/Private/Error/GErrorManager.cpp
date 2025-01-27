
#include "Error/GErrorManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GErrorManager)

bool UGErrorHelper::IsOk(const FGErrorInfo& Err)
{
	return GameCore::IsOK(Err);
}

FGErrorInfo UGErrorHelper::Pass()
{
	return GameCore::Pass();
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
