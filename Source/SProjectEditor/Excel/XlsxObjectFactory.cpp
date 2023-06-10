
#include "XlsxObjectFactory.h"
#include "Misc/MessageDialog.h"
#include "XlsxObject.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <WinNls.h>
#include <string>
#include <exception>
#include <set>
//! OpenXLSX Library
#include "OpenXLSX/OpenXLSX.hpp"
#include "OpenXLSX/headers/XLCellValue.hpp"
#endif

#if PLATFORM_WINDOWS
static std::string WStringToString(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[size];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, chRtn, size, NULL, NULL);
	std::string str(chRtn);
	delete[] chRtn;
	return str;
}

static FString UTF8ToTCHARString(const std::string& utf8Str)
{
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
}
#endif

UXlsxObjectFactory::UXlsxObjectFactory()
{
	SupportedClass = UXlsxObject::StaticClass();
	Formats.Add(TEXT("xlsx;Microsoft Excel Spreadsheet"));
	bEditorImport = true;
	bText = false;
	bCreateNew = true;
}

UObject* UXlsxObjectFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// TODO: 이미 존재한다면, 존재 중인 객체의 Object들 모두 삭제
	FString FileName = ThisClass::GetCurrentFilename();
	std::set<std::string> StructNames;
	std::set<std::string> EnumNames;

	try
	{
		OpenXLSX::XLDocument Document(::WStringToString(*FileName));
		if (!Document.isOpen())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to open the document. [%s]"), *FileName)));
			return nullptr;
		}

		OpenXLSX::XLWorkbook WorkBook = Document.workbook();

		for (std::string const& Name : WorkBook.sheetNames())
		{
			FString SheetName = FString(Name.c_str());
			if (SheetName.Left(1) == TEXT("!"))
			{	// Blueprint Structure Type
				StructNames.emplace(Name);
			}
			else if (SheetName.Left(1) == TEXT("@"))
			{	// Blueprint Enum Type
				EnumNames.emplace(Name);
			}
			else
			{	// Unknown
				continue;
			}
		}
	}
	catch (std::exception Exception)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Exception.what()));
	}

	try
	{
		// TODO: Create Enum types
	}
	catch (std::exception Exception)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Exception.what()));
	}

	UXlsxObject* Result = Cast<UXlsxObject>(CreateOrOverwriteAsset(InClass, InParent, InName, Flags));

	return Result;
}
