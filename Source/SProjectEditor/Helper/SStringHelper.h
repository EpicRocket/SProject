
#pragma once

#include "CoreMinimal.h"

#include <string>

namespace Helper
{
    std::string WStringToString(const std::wstring& wstr);

    FString UTF8ToTCHARString(const std::string& utf8Str);

    FString ExtractSubstring(const FString& FullString, const FString& StartDelimiter, const FString& EndDelimiter);
}   // Helper
