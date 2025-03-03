
#pragma once

#include "CoreMinimal.h"

#include <string>

namespace OpenXLSX {
    std::string WStringToString(const std::wstring& wstr);

    FString UTF8ToTCHARString(const std::string& utf8Str);
}
