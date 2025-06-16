

#pragma once

// include Engine
#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
// include GameCore
#include "Error/GError.h"
#include "Core/GContext.h"

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
