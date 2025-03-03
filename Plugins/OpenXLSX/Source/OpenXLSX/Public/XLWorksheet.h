
#pragma once

#include "CoreMinimal.h"

namespace OpenXLSX {
	class XLWorksheet;
}

class XLWorkbook;
class XLRow;

class OPENXLSX_API XLWorksheet {
public:
	TArray<TSharedPtr<XLRow>> Rows() const;

#if PLATFORM_WINDOWS
public:
	void SetWorksheet(OpenXLSX::XLWorksheet InWorksheet);
private:
	TSharedPtr<OpenXLSX::XLWorksheet> Worksheet;
#endif
};
