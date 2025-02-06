

#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

#define MY_API SPROJECT_API

DECLARE_LOG_CATEGORY_EXTERN(LogProject, Log, All);

namespace Global
{
template<typename T>
const T& GetEmpty()
{
	static T Empty;
	return Empty;
}
}
