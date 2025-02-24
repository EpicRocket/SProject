
#include "XLRow.h"

#include "OpenXLSXLogging.h"
#include "OpenXLSXUtil.h"
#include "XLCell.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#endif

TArray<TSharedPtr<XLCell>> XLRow::Cells() const
{
    TArray<TSharedPtr<XLCell>> Result;
#if PLATFORM_WINDOWS
	if (Row.IsValid())
	{
		for (auto& Cell : Row->cells())
		{
			TSharedPtr<XLCell> CellPtr = MakeShared<XLCell>();
			CellPtr->SetCell(Cell);
			Result.Add(CellPtr);
		}
	}
#endif
    return Result;
}

#if PLATFORM_WINDOWS
void XLRow::SetRow(OpenXLSX::XLRow InRow)
{
	Row = MakeShared<OpenXLSX::XLRow>(InRow);
}
#endif
