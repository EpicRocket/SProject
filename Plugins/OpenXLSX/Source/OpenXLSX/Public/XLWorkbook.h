
#pragma once

#include "CoreMinimal.h"

namespace OpenXLSX {
	class XLWorkbook;
}

class XLDocument;
class XLWorksheet;

class OPENXLSX_API XLWorkbook {
public:
	int32 WorksheetCount() const;

	TArray<FString> WorkSheetNames() const;

	TSharedPtr<XLWorksheet> GetWorksheet(const FString& SheetName);

	TMap<FString, TSharedPtr<XLWorksheet>> Worksheets;

#if PLATFORM_WINDOWS
public:
	void SetWorkbook(OpenXLSX::XLWorkbook InWorkbook);
private:
	TSharedPtr<OpenXLSX::XLWorkbook> Workbook;
#endif
};
