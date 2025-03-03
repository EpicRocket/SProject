
#include "XLWorkbook.h"

#include "OpenXLSXLogging.h"
#include "OpenXLSXUtil.h"
#include "XLWorksheet.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#endif

int32 XLWorkbook::WorksheetCount() const
{
	return Worksheets.Num();
}

TArray<FString> XLWorkbook::WorkSheetNames() const
{
	TArray<FString> Result;
	Worksheets.GetKeys(Result);
	return Result;
}

TSharedPtr<XLWorksheet> XLWorkbook::GetWorksheet(const FString& SheetName)
{
	auto Iter = Worksheets.Find(SheetName);
	if (!Iter)
	{
		return nullptr;
	}
	return *Iter;
}

#if PLATFORM_WINDOWS
void XLWorkbook::SetWorkbook(OpenXLSX::XLWorkbook InWorkbook)
{
	Workbook = MakeShared<OpenXLSX::XLWorkbook>(InWorkbook);

	Worksheets.Empty();
	try
	{
		for (auto& SheetName : Workbook->sheetNames())
		{
			FString SheetNameStr = OpenXLSX::UTF8ToTCHARString(SheetName);
			auto Worksheet = Workbook->worksheet(SheetName);

			TSharedPtr<XLWorksheet> WorksheetPtr = MakeShared<XLWorksheet>();
			WorksheetPtr->SetWorksheet(Worksheet);

			Worksheets.Emplace(SheetNameStr, WorksheetPtr);
		}
	}
	catch (std::exception& Exception)
	{
		UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
	}
}
#endif
