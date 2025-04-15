
#include "SStringHelper.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <WinNls.h>
#endif

namespace Helper
{
	std::string WStringToString(const std::wstring& wstr)
	{
#if PLATFORM_WINDOWS
		int size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		char* chRtn = new char[size];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, chRtn, size, NULL, NULL);
		std::string str(chRtn);
		delete[] chRtn;
		return str;
#else
		return std::string();
#endif
	}

	FString UTF8ToTCHARString(const std::string& utf8Str)
	{
#if PLATFORM_WINDOWS
		if (utf8Str.empty())
		{
			return FString();
		}

		int32_t targetLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
		if (targetLength == 0)
		{
			return FString();
		}

		TArray<TCHAR> targetArray;
		targetArray.Reserve(targetLength);
		MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, targetArray.GetData(), targetLength);
		return FString(targetArray.GetData());
#else
		return FString();
#endif
	}

	FString ExtractSubstring(const FString& FullString, const FString& StartDelimiter, const FString& EndDelimiter)
	{
		FString Result;
		int32 StartIndex = FullString.Find(StartDelimiter, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		if (StartIndex != INDEX_NONE)
		{
			int32 EndIndex = FullString.Find(EndDelimiter, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			if (EndIndex != INDEX_NONE)
			{
				Result = FullString.Mid(StartIndex + 1, EndIndex - (StartIndex + 1));
			}
		}
		return Result;
	}

} // Helper