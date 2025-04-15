
#include "XLWorksheet.h"

#include "OpenXLSXLogging.h"
#include "OpenXLSXUtil.h"
#include "XLRow.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#endif

TArray<TSharedPtr<XLRow>> XLWorksheet::Rows() const
{
	TArray<TSharedPtr<XLRow>> Result;
#if PLATFORM_WINDOWS
	if (Worksheet.IsValid())
	{
		for (auto& Row : Worksheet->rows())
		{
			TSharedPtr<XLRow> RowPtr = MakeShared<XLRow>();
			RowPtr->SetRow(Row);
			Result.Add(RowPtr);
		}
	}
#endif
	return Result;
}

#if PLATFORM_WINDOWS
void XLWorksheet::SetWorksheet(OpenXLSX::XLWorksheet InWorksheet)
{
	Worksheet = MakeShared<OpenXLSX::XLWorksheet>(InWorksheet);
}
#endif